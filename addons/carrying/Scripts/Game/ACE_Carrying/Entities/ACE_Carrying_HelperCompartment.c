//------------------------------------------------------------------------------------------------
class ACE_Carrying_HelperCompartmentClass : GenericEntityClass
{
}

//------------------------------------------------------------------------------------------------
//! Helper compartment entity that dynamically gets created/deleted and attached/detached to carriers
class ACE_Carrying_HelperCompartment : GenericEntity
{
	protected IEntity m_pCarrier;
	protected IEntity m_pCarried;
	protected SCR_CharacterControllerComponent m_CarrierCharCtrl;
	protected static EPhysicsLayerPresets m_iPhysicsLayerPreset = -1;
	protected static const int SEARCH_POS_RADIUS_M = 5; // Search radius for safe position for dropping carried player
	protected static const float HELPER_DELETION_DELAY_MS = 1000; // Delay for helper to get deleted after release
	protected static const ref array<string> WALKING_INPUT_ACTION_NAMES = {"CharacterForward", "CharacterRight"};
	protected static const float WALKING_INPUT_ACTION_LIMIT = 0.5;
	
	//------------------------------------------------------------------------------------------------
	//! Start <carrier> to carry the specified <carried>
	//! This helper compartment is attached to the <carrier> and the <carried> moves inside
	void Init(IEntity carrier, IEntity carried)
	{
		m_pCarrier = carrier;
		m_pCarried = carried;
		
		carrier.AddChild(this, carrier.GetAnimation().GetBoneIndex("Spine5"));

		PlayerManager playerManager = GetGame().GetPlayerManager();
		SCR_PlayerController carrierPlayerController = SCR_PlayerController.Cast(playerManager.GetPlayerController(playerManager.GetPlayerIdFromControlledEntity(carrier)));
		
		RplComponent rpl = RplComponent.Cast(FindComponent(RplComponent));
		if (!rpl)
			return;
		
		rpl.Give(carrierPlayerController.GetRplIdentity());
		
		SCR_CharacterControllerComponent carrierController = SCR_CharacterControllerComponent.Cast(carrier.FindComponent(SCR_CharacterControllerComponent));
		if (!carrierController)
			return;
		
		carrierController.m_OnLifeStateChanged.Insert(OnCarrierLifeStateChanged);
		
		SCR_CharacterControllerComponent carriedController = SCR_CharacterControllerComponent.Cast(carried.FindComponent(SCR_CharacterControllerComponent));
		if (!carriedController)
			return;
		
		carriedController.m_OnLifeStateChanged.Insert(OnCarriedLifeStateChanged);

		RplComponent carriedRpl = RplComponent.Cast(carried.FindComponent(RplComponent));
		if (!carriedRpl)
			return;
		
		Rpc(RpcDo_Owner_Init, carriedRpl.Id());
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(carried.FindComponent(SCR_CompartmentAccessComponent));
		if (!compartmentAccess)
			return;
		
		compartmentAccess.MoveInVehicle(this, ECompartmentType.CARGO);
	}

	//------------------------------------------------------------------------------------------------
	//! Called on carrier's machine
	//! - Add release action keybind
	//! - Disable physcial interaction between carrier and carried player
	//! - Add prone prevention handling
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_Owner_Init(RplId carriedId)
	{
		GetGame().GetInputManager().AddActionListener("ACE_Carrying_Release", EActionTrigger.DOWN, ActionTerminateCallback);
		IEntity carried = RplComponent.Cast(Replication.FindItem(carriedId)).GetEntity();
		Physics carriedPhys = carried.GetPhysics();
		
		if (m_iPhysicsLayerPreset < 0)
			m_iPhysicsLayerPreset = carriedPhys.GetInteractionLayer();
		
		carriedPhys.SetInteractionLayer(EPhysicsLayerPresets.FireGeo);
		SCR_PlayerController carrierCtrl = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		ChimeraCharacter carrier = ChimeraCharacter.Cast(carrierCtrl.GetControlledEntity());
		m_CarrierCharCtrl = SCR_CharacterControllerComponent.Cast(carrier.GetCharacterController());
		SetEventMask(EntityEvent.FRAME);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Limit walking speed and prevent prone stance
	override protected void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
		
		foreach (string actionName : WALKING_INPUT_ACTION_NAMES)
		{
			float value = GetGame().GetInputManager().GetActionValue(actionName);
			GetGame().GetInputManager().SetActionValue(actionName, Math.Clamp(value, -WALKING_INPUT_ACTION_LIMIT, WALKING_INPUT_ACTION_LIMIT));
		}
		
		// Reset the stance if player attempts to go prone
		if (m_CarrierCharCtrl.GetStance() == ECharacterStance.PRONE)
			m_CarrierCharCtrl.SetStanceChange(ECharacterStanceChange.STANCECHANGE_TOCROUCH);
	}
	
	//------------------------------------------------------------------------------------------------
	// Release from carrier when they get incapacitated or die
	protected void OnCarrierLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		ACE_Carrying_Tools.ReleaseFromCarrier(m_pCarrier);
	}
	
	//------------------------------------------------------------------------------------------------
	// Release carried when they wake up or die
	protected void OnCarriedLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		ACE_Carrying_Tools.ReleaseCarried(m_pCarried);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminates carrying: Moves out the carried player and schedules clean up
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Terminate()
	{
		if (m_pCarrier)
		{
			m_pCarrier.RemoveChild(this, true);
			
			SCR_CharacterControllerComponent carrierController = SCR_CharacterControllerComponent.Cast(m_pCarrier.FindComponent(SCR_CharacterControllerComponent));
			if (!carrierController)
				return;
			
			carrierController.m_OnLifeStateChanged.Remove(OnCarrierLifeStateChanged);
		}

		if (m_pCarried)
		{
			MoveOutCarried();
			
			SCR_CharacterControllerComponent carriedController = SCR_CharacterControllerComponent.Cast(m_pCarried.FindComponent(SCR_CharacterControllerComponent));
			if (!carriedController)
				return;
		
			carriedController.m_OnLifeStateChanged.Remove(OnCarriedLifeStateChanged);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Moves the carried player out of the helper compartment
	protected void MoveOutCarried()
	{
		if (!m_pCarried)
			return;
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_pCarried.FindComponent(SCR_CompartmentAccessComponent));
		if (!compartmentAccess)
			return;
		
		// Clean-up when carried has left the comparment
		compartmentAccess.GetOnCompartmentLeft().Insert(OnCompartmentLeft);
		
		vector target_pos;
		vector target_transform[4];
		m_pCarrier.GetWorldTransform(target_transform);
		// target_transform[2] is vectorDir in Arma 3
		SCR_WorldTools.FindEmptyTerrainPosition(target_pos, target_transform[3] + target_transform[2], SEARCH_POS_RADIUS_M);
		target_transform[3] = target_pos;
		compartmentAccess.ACE_MoveOutVehicle(target_transform);
		
		// Broadcast teleport on network
		RplComponent carriedRpl = RplComponent.Cast(m_pCarried.FindComponent(RplComponent));
		if (carriedRpl)
			carriedRpl.ForceNodeMovement(GetOrigin());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clean-up when the carried player has left the compartment
	protected void OnCompartmentLeft()
	{
		RplId carriedId = RplId.Invalid();
		
		if (m_pCarried)
		{
			SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_pCarried.FindComponent(SCR_CompartmentAccessComponent));
			if (compartmentAccess)
				compartmentAccess.GetOnCompartmentLeft().Remove(OnCompartmentLeft);
			
			RplComponent carriedRpl = RplComponent.Cast(m_pCarried.FindComponent(RplComponent));
			carriedId = carriedRpl.Id();
		};
		
		Rpc(RpcDo_Owner_CleanUp, carriedId);
		// Deletion of helper has to be delayed or released players stay visibly prone for other players on dedicated
		GetGame().GetCallqueue().CallLater(SCR_EntityHelper.DeleteEntityAndChildren, HELPER_DELETION_DELAY_MS, false, this);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clean-up on the carrier
	//! - Remove release keybinds
	//! - Enable physical interaction between carrier and carried player
	//! - Remove prone prevention handling
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_Owner_CleanUp(RplId carriedId)
	{
		GetGame().GetInputManager().RemoveActionListener("ACE_Carrying_Release", EActionTrigger.DOWN, ActionTerminateCallback);
		
		IEntity carried = RplComponent.Cast(Replication.FindItem(carriedId)).GetEntity();
		if (carried)
			carried.GetPhysics().SetInteractionLayer(m_iPhysicsLayerPreset);
		
		ClearEventMask(EntityEvent.FRAME);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Callback for the release keybind
	protected void ActionTerminateCallback()
	{
		Rpc(Terminate);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns the carrier
	IEntity GetCarrier()
	{
		return m_pCarrier;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns the carried unit
	IEntity GetCarried()
	{
		return m_pCarried;
	}
}
