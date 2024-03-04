//------------------------------------------------------------------------------------------------
class ACE_Carrying_HelperClass : GenericEntityClass
{
}

//------------------------------------------------------------------------------------------------
//! Helper compartment entity that dynamically gets created/deleted and attached/detached to carriers
class ACE_Carrying_Helper : GenericEntity
{
	protected IEntity m_eCarrier;
	protected IEntity m_eCarried;
	protected SCR_CharacterControllerComponent m_CarrierCharCtrl;
	protected static EPhysicsLayerPresets m_iPhysicsLayerPreset = -1;
	private static const ResourceName HELPER_PREFAB_NAME = "{FF78613C1DAFF28F}Prefabs/Helpers/ACE_Carrying_Helper.et";
	private static const int SEARCH_POS_RADIUS = 5; // m
	private static const float PRONE_CHECK_TIMEOUT_MS = 100; // ms
	
	//------------------------------------------------------------------------------------------------
	//! Start <carrier> to carry the specified <carried>
	//! Creates an instance of the helper compartment, attaches it to the <carrier> and moves the <carried>
	//! in the compartment
	static void Carry(IEntity carrier, IEntity carried)
	{
		ACE_Carrying_Helper helper = ACE_Carrying_Helper.Cast(GetGame().SpawnEntityPrefab(Resource.Load(HELPER_PREFAB_NAME), null, EntitySpawnParams()));
		helper.m_eCarrier = carrier;
		helper.m_eCarried = carried;
						
		carrier.AddChild(helper, carrier.GetAnimation().GetBoneIndex("Spine5"));
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(carried.FindComponent(SCR_CompartmentAccessComponent));
		if (!compartmentAccess)
			return;
		
		PlayerManager playerManager = GetGame().GetPlayerManager();
		SCR_PlayerController carrierCtrl = SCR_PlayerController.Cast(playerManager.GetPlayerController(playerManager.GetPlayerIdFromControlledEntity(carrier)));
		RplComponent helperRpl = RplComponent.Cast(helper.FindComponent(RplComponent));
		RplId carrierCtrlId = carrierCtrl.GetRplIdentity();
		helperRpl.Give(carrierCtrlId);

		RplId carriedId = RplComponent.Cast(carried.FindComponent(RplComponent)).Id();
		helper.Rpc(helper.RpcDo_Owner_Carry, carriedId);
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
		GetGame().GetCallqueue().CallLater(PreventProneCarrier, PRONE_CHECK_TIMEOUT, true);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Resets the stance if player attempts to go prone
	protected void PreventProneCarrier()
	{
		if (m_CarrierCharCtrl.GetStance() == ECharacterStance.PRONE)
			m_CarrierCharCtrl.SetStanceChange(ECharacterStanceChange.STANCECHANGE_TOCROUCH);
	}

	//------------------------------------------------------------------------------------------------
	//! Release the carried player by passing the carrier
	//! Calls Release method on helper compartment entity
	static void ReleaseFromCarrier(IEntity carrier)
	{
		ACE_Carrying_Helper helper = GetHelperFromCarrier(carrier);
		
		if (!helper)
			return;
		
		helper.Release();
	}

	//------------------------------------------------------------------------------------------------
	//! Release the carried player
	//! Calls Release method on helper compartment entity
	static void ReleaseCarried(IEntity carried)
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
		m_eCarrier.RemoveChild(this, true);
		MoveOutCarried();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Moves the carried player out of the helper compartment
	protected void MoveOutCarried()
	{
		if (!m_eCarried)
			return;
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_eCarried.FindComponent(SCR_CompartmentAccessComponent));
		if (!compartmentAccess)
			return;
		
		// Clean-up when carried has left the comparment
		compartmentAccess.GetOnCompartmentLeft().Insert(OnCompartmentLeft);
		
		vector target_pos;
		vector target_transform[4];
		m_eCarrier.GetWorldTransform(target_transform);
		// target_transform[2] is vectorDir in Arma 3
		SCR_WorldTools.FindEmptyTerrainPosition(target_pos, target_transform[3] + target_transform[2], SEARCH_POS_RADIUS);
		target_transform[3] = target_pos;
		compartmentAccess.MoveOutVehicle(-1, target_transform);
		
		// Broadcast teleport on network
		RplComponent carriedRpl =  RplComponent.Cast(m_eCarried.FindComponent(RplComponent));
		if (carriedRpl)
			carriedRpl.ForceNodeMovement(GetOrigin());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clean-up when the carried player has left the compartment
	protected void OnCompartmentLeft()
	{
		RplId carriedId = RplId.Invalid();
		
		if (m_eCarried)
		{
			SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(m_eCarried.FindComponent(SCR_CompartmentAccessComponent));
			if (compartmentAccess)
				compartmentAccess.GetOnCompartmentLeft().Remove(OnCompartmentLeft);
			
			RplComponent carriedRpl =  RplComponent.Cast(m_eCarried.FindComponent(RplComponent));
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
	static bool IsCarrier(IEntity carrier)
	{
		return GetHelperFromCarrier(carrier);
	}
	
	//------------------------------------------------------------------------------------------------
	//! True if the given player is currently carried by another player
	static bool IsCarried(IEntity carried)
	{
		return GetHelperFromCarried(carried);
	}

	//------------------------------------------------------------------------------------------------
	//! Get the player that is carried by the given carrier
	static IEntity GetCarried(IEntity carrier)
	{
		ACE_Carrying_Helper helper = GetHelperFromCarrier(carrier);
		if (!helper)
			return null;
		
		return helper.m_eCarried;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get the carrier that carries the given player
	static IEntity GetCarrier(IEntity carried)
	{
		ACE_Carrying_Helper helper = GetHelperFromCarried(carried);
		if (!helper)
			return null;
		
		return helper.m_eCarrier;
	}

	//------------------------------------------------------------------------------------------------
	//! Get the instance of the helper compartment entity for the given carrier
	protected static ACE_Carrying_Helper GetHelperFromCarrier(IEntity carrier)
	{
		if (!carrier)
			return null;
		
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
	protected static ACE_Carrying_Helper GetHelperFromCarried(IEntity carried)
	{
		if (!carried)
			return null;
		
		return ACE_Carrying_Helper.Cast(carried.GetParent());
	}
}
