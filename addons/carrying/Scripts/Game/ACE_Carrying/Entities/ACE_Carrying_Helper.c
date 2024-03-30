//------------------------------------------------------------------------------------------------
class ACE_Carrying_HelperClass : GenericEntityClass
{
}

//------------------------------------------------------------------------------------------------
//! Helper compartment entity that dynamically gets created/deleted and attached/detached to carriers
class ACE_Carrying_Helper : GenericEntity
{
	protected IEntity m_pCarrier;
	protected IEntity m_pCarried;
	protected SCR_CharacterControllerComponent m_CarrierCharCtrl;
	protected static EPhysicsLayerPresets m_iPhysicsLayerPreset = -1;
	private static const ResourceName HELPER_PREFAB_NAME = "{FF78613C1DAFF28F}Prefabs/Helpers/ACE_Carrying_Helper.et";
	private static const int SEARCH_POS_RADIUS_M = 5; // Search radius for safe position for dropping carried player
	private static const float PRONE_CHECK_TIMEOUT_MS = 100; // Timeout for checking whether carrier tries do go prone
	
	//------------------------------------------------------------------------------------------------
	//! Start <carrier> to carry the specified <carried>
	//! Creates an instance of the helper compartment, attaches it to the <carrier> and moves the <carried>
	//! in the compartment
	static void Carry(notnull IEntity carrier, notnull IEntity carried)
	{
		ACE_Carrying_Helper helper = ACE_Carrying_Helper.Cast(GetGame().SpawnEntityPrefab(Resource.Load(HELPER_PREFAB_NAME), null, EntitySpawnParams()));
		helper.m_pCarrier = carrier;
		helper.m_pCarried = carried;
						
		carrier.AddChild(helper, carrier.GetAnimation().GetBoneIndex("Spine5"));
		PlayerManager playerManager = GetGame().GetPlayerManager();
		SCR_PlayerController carrierCtrl = SCR_PlayerController.Cast(playerManager.GetPlayerController(playerManager.GetPlayerIdFromControlledEntity(carrier)));
		RplComponent helperRpl = RplComponent.Cast(helper.FindComponent(RplComponent));
		RplId carrierCtrlId = carrierCtrl.GetRplIdentity();
		helperRpl.Give(carrierCtrlId);
		
		SCR_CharacterControllerComponent carrierController = SCR_CharacterControllerComponent.Cast(carrier.FindComponent(SCR_CharacterControllerComponent));
		if (!carrierController)
			return;
		
		carrierController.m_OnLifeStateChanged.Insert(helper.OnCarrierLifeStateChanged);
		
		SCR_CharacterControllerComponent carriedController = SCR_CharacterControllerComponent.Cast(carried.FindComponent(SCR_CharacterControllerComponent));
		if (!carriedController)
			return;
		
		carriedController.m_OnLifeStateChanged.Insert(helper.OnCarriedLifeStateChanged);

		RplId carriedId = RplComponent.Cast(carried.FindComponent(RplComponent)).Id();
		helper.Rpc(helper.RpcDo_Owner_Carry, carriedId);
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(carried.FindComponent(SCR_CompartmentAccessComponent));
		if (!compartmentAccess)
			return;
		
		compartmentAccess.MoveInVehicle(helper, ECompartmentType.Cargo);
	}

	//------------------------------------------------------------------------------------------------
	//! Called on carrier's machine
	//! - Add release action keybind
	//! - Disable physcial interaction between carrier and carried player
	//! - Add prone prevention handling
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_Owner_Carry(RplId carriedId)
	{
		GetGame().GetInputManager().AddActionListener("ACE_Carrying_Release", EActionTrigger.DOWN, ActionReleaseCallback);
		IEntity carried = RplComponent.Cast(Replication.FindItem(carriedId)).GetEntity();
		Physics carriedPhys = carried.GetPhysics();
		
		if (m_iPhysicsLayerPreset < 0)
			m_iPhysicsLayerPreset = carriedPhys.GetInteractionLayer();
		
		carriedPhys.SetInteractionLayer(EPhysicsLayerPresets.FireGeo);
		SCR_PlayerController carrierCtrl = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		ChimeraCharacter carrier = ChimeraCharacter.Cast(carrierCtrl.GetControlledEntity());
		m_CarrierCharCtrl = SCR_CharacterControllerComponent.Cast(carrier.GetCharacterController());
		GetGame().GetCallqueue().CallLater(PreventProneCarrier, PRONE_CHECK_TIMEOUT_MS, true);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Resets the stance if player attempts to go prone
	protected void PreventProneCarrier()
	{
		if (m_CarrierCharCtrl.GetStance() == ECharacterStance.PRONE)
			m_CarrierCharCtrl.SetStanceChange(ECharacterStanceChange.STANCECHANGE_TOCROUCH);
	}
	
	//------------------------------------------------------------------------------------------------
	// Release from carrier when they get incapacitated or die
	protected void OnCarrierLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		ACE_Carrying_Helper.ReleaseFromCarrier(m_pCarrier);
	}
	
	//------------------------------------------------------------------------------------------------
	// Release carried when they wake up or die
	protected void OnCarriedLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		ACE_Carrying_Helper.ReleaseCarried(m_pCarried);
	}

	//------------------------------------------------------------------------------------------------
	//! Release the carried player by passing the carrier
	//! Calls Release method on helper compartment entity
	static void ReleaseFromCarrier(notnull IEntity carrier)
	{
		ACE_Carrying_Helper helper = GetHelperFromCarrier(carrier);
		
		if (!helper)
			return;
		
		helper.Release();
	}

	//------------------------------------------------------------------------------------------------
	//! Release the carried player
	//! Calls Release method on helper compartment entity
	static void ReleaseCarried(notnull IEntity carried)
	{
		ACE_Carrying_Helper helper = GetHelperFromCarried(carried);
		
		if (!helper)
			return;
		
		helper.Release();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Release method for the helper compartment entity
	//! Moves out the carried player and schedules clean up
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Release()
	{
		m_pCarrier.RemoveChild(this, true);
		MoveOutCarried();
		
		if (m_pCarrier)
		{
			SCR_CharacterControllerComponent carrierController = SCR_CharacterControllerComponent.Cast(m_pCarrier.FindComponent(SCR_CharacterControllerComponent));
			if (!carrierController)
				return;
			
			carrierController.m_OnLifeStateChanged.Remove(OnCarrierLifeStateChanged);
		}

		if (m_pCarried)
		{
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
		compartmentAccess.MoveOutVehicle(-1, target_transform);
		
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
		SCR_EntityHelper.DeleteEntityAndChildren(this);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clean-up on the carrier
	//! - Remove release keybinds
	//! - Enable physical interaction between carrier and carried player
	//! - Remove prone prevention handling
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_Owner_CleanUp(RplId carriedId)
	{
		GetGame().GetInputManager().RemoveActionListener("ACE_Carrying_Release", EActionTrigger.DOWN, ActionReleaseCallback);
		
		IEntity carried = RplComponent.Cast(Replication.FindItem(carriedId)).GetEntity();
		if (carried)
			carried.GetPhysics().SetInteractionLayer(m_iPhysicsLayerPreset);
		
		GetGame().GetCallqueue().Remove(PreventProneCarrier);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Callback for the release keybind
	protected void ActionReleaseCallback()
	{
		Rpc(Release);
	}
	
	//------------------------------------------------------------------------------------------------
	//! True if the given player is currently a carrier
	static bool IsCarrier(notnull IEntity carrier)
	{
		return GetHelperFromCarrier(carrier);
	}
	
	//------------------------------------------------------------------------------------------------
	//! True if the given player is currently carried by another player
	static bool IsCarried(notnull IEntity carried)
	{
		return GetHelperFromCarried(carried);
	}

	//------------------------------------------------------------------------------------------------
	//! Get the player that is carried by the given carrier
	static IEntity GetCarried(notnull IEntity carrier)
	{
		ACE_Carrying_Helper helper = GetHelperFromCarrier(carrier);
		if (!helper)
			return null;
		
		return helper.m_pCarried;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get the carrier that carries the given player
	static IEntity GetCarrier(notnull IEntity carried)
	{
		ACE_Carrying_Helper helper = GetHelperFromCarried(carried);
		if (!helper)
			return null;
		
		return helper.m_pCarrier;
	}

	//------------------------------------------------------------------------------------------------
	//! Get the instance of the helper compartment entity for the given carrier
	protected static ACE_Carrying_Helper GetHelperFromCarrier(notnull IEntity carrier)
	{
		ACE_Carrying_Helper helper;
		IEntity child = carrier.GetChildren();
		
		while (child)
		{
			helper = ACE_Carrying_Helper.Cast(child);
			
			if (helper)
				break;
			
			child = child.GetSibling();
		};
		
		return helper;
	}

	//------------------------------------------------------------------------------------------------
	//! Get the instance of the helper compartment entity for the given carried player
	protected static ACE_Carrying_Helper GetHelperFromCarried(notnull IEntity carried)
	{
		return ACE_Carrying_Helper.Cast(carried.GetParent());
	}
}
