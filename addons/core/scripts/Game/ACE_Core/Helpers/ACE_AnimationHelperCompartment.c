//------------------------------------------------------------------------------------------------
class ACE_AnimationHelperCompartmentClass : GenericEntityClass
{
}

//------------------------------------------------------------------------------------------------
//! Helper compartment for performing a continous animation or pose while a character occupies it
//! --- To Do: Create derived classes of this for carrying and CPR
class ACE_AnimationHelperCompartment : GenericEntity
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, desc: "ID of the animation helper",  enums: ParamEnumArray.FromEnum(ACE_EAnimationHelperID))]
	protected ACE_EAnimationHelperID m_eID;
	
	protected IEntity m_pPerformer;
	protected ref ScriptInvokerVoid m_OnTerminated;

	protected static const int SEARCH_POS_RADIUS_M = 5; // Search radius for safe position for dropping performer
	protected static const float HELPER_DELETION_DELAY_MS = 1000; // Delay for helper to get deleted after release
	
	//------------------------------------------------------------------------------------------------
	//! Move performer into compartment and attach handlers
	void Init(IEntity performer)
	{
		m_pPerformer = performer;
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(performer.FindComponent(SCR_CompartmentAccessComponent));
		if (!compartmentAccess)
			return;
		
		compartmentAccess.GetOnCompartmentEntered().Insert(OnCompartmentEntered);
		compartmentAccess.MoveInVehicle(this, ECompartmentType.CARGO);
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
	protected void AttachHandlers()
	{
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_pPerformer.FindComponent(SCR_CharacterControllerComponent));
		if (charController)
			charController.m_OnLifeStateChanged.Insert(OnPerformerLifeStateChanged);
				
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (gameMode)
			gameMode.GetOnPlayerDisconnected().Insert(OnPlayerDisconnected);
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_pPerformer.FindComponent(SCR_CompartmentAccessComponent));
		if (compartmentAccess)
			compartmentAccess.GetOnCompartmentLeft().Insert(OnCompartmentLeft);
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
		
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (gameMode)
			gameMode.GetOnPlayerDisconnected().Remove(OnPlayerDisconnected);
		
		// Clean-up when carried has left the compartment
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_pPerformer.FindComponent(SCR_CompartmentAccessComponent));
		if (compartmentAccess)
			compartmentAccess.GetOnCompartmentLeft().Remove(OnCompartmentLeft);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminates animation
	void Terminate(EGetOutType getOutType)
	{
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_pPerformer.FindComponent(SCR_CompartmentAccessComponent));
		if (!compartmentAccess)
			return;
		
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
		Terminate(EGetOutType.TELEPORT);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminate when performer or patient disconnects
	protected void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		if (player == m_pPerformer)
			Terminate(EGetOutType.TELEPORT);
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity GetPerformer()
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
	
	//------------------------------------------------------------------------------------------------
	ACE_EAnimationHelperID GetHelperID()
	{
		return m_eID;
	}
}
