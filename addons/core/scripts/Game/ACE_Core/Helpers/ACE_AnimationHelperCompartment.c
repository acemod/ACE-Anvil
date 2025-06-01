//------------------------------------------------------------------------------------------------
class ACE_AnimationHelperCompartmentClass : GenericEntityClass
{
}

//------------------------------------------------------------------------------------------------
//! Helper compartment for performing a continous animation or pose while a character occupies it
//! --- To Do: Create derived classes of this for carrying and CPR
class ACE_AnimationHelperCompartment : GenericEntity
{
	[Attribute(defvalue: "ANIMATED", uiwidget: UIWidgets.SearchComboBox, desc: "Get out type when life state has changed", enums: ParamEnumArray.FromEnum(EGetOutType))]
	protected EGetOutType m_eLifeStateChangedGetOutType;
	
	[RplProp(onRplName: "OnPerformerChanged")]
	protected RplId m_iPerformerID;
	protected SCR_ChimeraCharacter m_pPerformer;
	
	protected bool m_bInitDone = false;
	protected ref ScriptInvokerVoid m_OnTerminated;

	protected static const int SEARCH_POS_RADIUS_M = 5; // Search radius for safe position for dropping performer
	protected static const float HELPER_DELETION_DELAY_MS = 1000; // Delay for helper to get deleted after release
	
	//------------------------------------------------------------------------------------------------
	//! Move performer into compartment and attach handlers
	void Init(SCR_ChimeraCharacter performer)
	{
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(performer.FindComponent(SCR_CompartmentAccessComponent));
		if (compartmentAccess)
		{
			// Already signal the access component that we are about to move into the helper compartment
			compartmentAccess.ACE_SetIsRequestingGettingIn(true);
			compartmentAccess.GetOnCompartmentEntered().Insert(OnCompartmentEntered);
		}
		
		m_pPerformer = performer;
		m_iPerformerID = Replication.FindItemId(m_pPerformer);
		OnPerformerChanged();
		Replication.BumpMe();
	}
		
	//------------------------------------------------------------------------------------------------
	protected void OnCompartmentEntered(IEntity targetEntity, BaseCompartmentManagerComponent manager, int mgrID, int slotID, bool move)
	{
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_pPerformer.FindComponent(SCR_CompartmentAccessComponent));
		if (!compartmentAccess)
			return;
		
		compartmentAccess.GetOnCompartmentEntered().Remove(OnCompartmentEntered);
		AttachHandlers();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Note that we do not need to handle OnPlayerDisconnected, since a disconnected occupant automatically
	//! triggers OnCompartmentLeft when they get deleted
	protected void AttachHandlers()
	{
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_pPerformer.FindComponent(SCR_CharacterControllerComponent));
		if (charController)
			charController.m_OnLifeStateChanged.Insert(OnPerformerLifeStateChanged);
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_pPerformer.FindComponent(SCR_CompartmentAccessComponent));
		if (compartmentAccess)
			compartmentAccess.GetOnCompartmentLeft().Insert(OnCompartmentLeft);
		
		m_bInitDone = true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DetachHandlers()
	{
		if (m_pPerformer)
		{
			SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_pPerformer.FindComponent(SCR_CharacterControllerComponent));
			if (charController)
				charController.m_OnLifeStateChanged.Remove(OnPerformerLifeStateChanged);
		}
		
		// Clean-up when carried has left the compartment
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_pPerformer.FindComponent(SCR_CompartmentAccessComponent));
		if (compartmentAccess)
			compartmentAccess.GetOnCompartmentLeft().Remove(OnCompartmentLeft);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminates animation
	void Terminate(EGetOutType getOutType = EGetOutType.ANIMATED)
	{
		// Reschedule termination when init is not yet done, for instance, when called before the character is moving in
		if (!m_bInitDone)
			GetGame().GetCallqueue().CallLater(Terminate, 100, false, getOutType);
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_pPerformer.FindComponent(SCR_CompartmentAccessComponent));
		if (!compartmentAccess)
		{
			OnCompartmentLeft();
			return;
		}
		
		if (getOutType == EGetOutType.ANIMATED)
			compartmentAccess.ACE_GetOutVehicle(EGetOutType.ANIMATED, -1, ECloseDoorAfterActions.INVALID, false);
		else
			EjectPerformer();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Eject the performer out of the helper compartment
	protected void EjectPerformer()
	{
		if (!m_pPerformer)
			return;
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_pPerformer.FindComponent(SCR_CompartmentAccessComponent));
		if (!compartmentAccess)
			return;
				
		vector transform[4];
		GetWorldTransform(transform);
		vector pos;
		SCR_WorldTools.FindEmptyTerrainPosition(pos, transform[3], SEARCH_POS_RADIUS_M);
		transform[3] = pos;
		compartmentAccess.ACE_MoveOutVehicle(transform);
		
		// Broadcast teleport on network
		RplComponent performerRpl = RplComponent.Cast(m_pPerformer.FindComponent(RplComponent));
		if (performerRpl)
			performerRpl.ForceNodeMovement(GetOrigin());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clean-up when the peformer has left the compartment
	protected void OnCompartmentLeft()
	{
		DetachHandlers();
		
		if (m_OnTerminated)
			m_OnTerminated.Invoke();
		
		// Deletion of helper has to be delayed or released players stay visibly prone for other players on dedicated
		GetGame().GetCallqueue().CallLater(SCR_EntityHelper.DeleteEntityAndChildren, HELPER_DELETION_DELAY_MS, false, this);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminate when performer gets incapacitated or dies
	protected void OnPerformerLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		Terminate(m_eLifeStateChangedGetOutType);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnPerformerChanged()
	{
		m_pPerformer = SCR_ChimeraCharacter.Cast(Replication.FindItem(m_iPerformerID));
		if (!m_pPerformer)
			return;
		
		RplComponent performerRpl = m_pPerformer.GetRplComponent();
		if (!performerRpl || !performerRpl.IsOwner())
			return;
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_pPerformer.FindComponent(SCR_CompartmentAccessComponent));
		if (!compartmentAccess)
			return;
		
		compartmentAccess.ACE_GetInVehicle(this);
	}
	
	//------------------------------------------------------------------------------------------------
	SCR_ChimeraCharacter GetPerformer()
	{
		return m_pPerformer;
	}
	
	//------------------------------------------------------------------------------------------------
	ScriptInvokerVoid GetOnTerminated()
	{
		if (!m_OnTerminated)
			m_OnTerminated = new ScriptInvokerVoid();
		
		return m_OnTerminated;
	}
}
