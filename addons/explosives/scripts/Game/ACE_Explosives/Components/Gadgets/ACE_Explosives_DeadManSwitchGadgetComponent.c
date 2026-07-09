//------------------------------------------------------------------------------------------------
class ACE_Explosives_DeadManSwitchGadgetComponentClass : SCR_DetonatorGadgetComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_Explosives_DeadManSwitchGadgetComponent : SCR_DetonatorGadgetComponent
{
	protected static ACE_Explosives_Settings s_Settings;
	protected ref array<IEntity> m_aConnectedInventoryCharges = {};
	
	protected static const string TARGET_BONE_NAME = "Stomach2"; // Bone to which inventory explosives get attached to
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!s_Settings)
			s_Settings = ACE_SettingsHelperT<ACE_Explosives_Settings>.GetModSettings();
	}
	
	//------------------------------------------------------------------------------------------------
	void OnOwnerIncapacitatedServer()
	{
		if (m_iNumberOfConnectedCharges <= 0)
			return;
		
		if (s_Settings.m_bDeadManSwitchOnlyActiveInHand)
		{
			SCR_GadgetManagerComponent manager = SCR_GadgetManagerComponent.Cast(m_CharacterOwner.FindComponent(SCR_GadgetManagerComponent));
			if (!manager || manager.GetHeldGadgetComponent() != this)
				return;
		}
		
		Rpc(RpcDo_OnOwnerIncapacitatedBroadcast);
		RpcDo_OnOwnerIncapacitatedBroadcast();
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_OnOwnerIncapacitatedBroadcast()
	{
		m_User = m_CharacterOwner;
		DetonateExplosiveCharge();
	}
	
	//------------------------------------------------------------------------------------------------
	void ConnectToInventoryCharges(array<IEntity> charges)
	{
		foreach (IEntity charge : charges)
		{
			SCR_ExplosiveChargeComponent explosive = SCR_ExplosiveChargeComponent.Cast(charge.FindComponent(SCR_ExplosiveChargeComponent));
			if (!explosive)
				continue;

			ConnectNewCharge(explosive.GetRplId());
			explosive.ConnectDetonator(SCR_EFuzeType.REMOTE, Replication.FindItemId(this));
			
			if (Replication.IsServer())
				m_aConnectedInventoryCharges.Insert(charge);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void RemoveAllChargesFromTheList()
	{
		super.RemoveAllChargesFromTheList();
		
		if (Replication.IsServer())
			m_aConnectedInventoryCharges.Clear();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Drop and detonate inventory charges
	override protected void DetonateExplosiveCharge()
	{
		if (!m_CharacterOwner)
			return;
		
		if (Replication.IsServer())
		{
			SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(m_CharacterOwner.FindComponent(SCR_InventoryStorageManagerComponent));
			if (!inventoryManager)
				return;
	
			Animation charAnim = m_CharacterOwner.GetAnimation();
			if (!charAnim)
				return;
			
			TNodeId targetBoneId = charAnim.GetBoneIndex(TARGET_BONE_NAME);
			
			foreach (IEntity explosive : m_aConnectedInventoryCharges)
			{
				if (!explosive)
					continue;
				
				RplComponent explosiveRpl = RplComponent.Cast(explosive.FindComponent(RplComponent));
				if (!explosiveRpl)
					continue;
				
				SCR_ExplosiveChargeInventoryItemComponent itemComponent = SCR_ExplosiveChargeInventoryItemComponent.Cast(explosive.FindComponent(SCR_ExplosiveChargeInventoryItemComponent));
				if (!itemComponent)
					continue;
				
				InventoryStorageSlot parentSlot = itemComponent.GetParentSlot();
				if (!parentSlot)
					continue;
				
				vector charTransform[4];
				m_CharacterOwner.GetWorldTransform(charTransform);
				vector localtargetTransform[4];
				charAnim.GetBoneMatrix(targetBoneId, localtargetTransform);
				vector targetTransform[4];
				Math3D.MatrixMultiply4(charTransform, localtargetTransform, targetTransform);
				itemComponent.SetPlacementPosition(targetTransform[0], targetTransform[1], targetTransform[2], targetTransform[3], RplId.Invalid());
				itemComponent.RequestUserLock(itemComponent.GetOwner(), false);
				inventoryManager.TryRemoveItemFromStorage(explosive, parentSlot.GetStorage());
			}
		}
		
		// Delay detonation to ensure that inventory charges have a proxy
		GetGame().GetCallqueue().Call(DetonateExplosiveChargeDelayed);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void DetonateExplosiveChargeDelayed()
	{
		super.DetonateExplosiveCharge();
		
		if (!Replication.IsServer())
			return;
			
		foreach (IEntity charge : m_aConnectedInventoryCharges)
		{
			SCR_ExplosiveChargeComponent explosive = SCR_ExplosiveChargeComponent.Cast(charge.FindComponent(SCR_ExplosiveChargeComponent));
			if (!explosive)
				continue;
			
			SCR_ExplosiveTriggerComponent trigger = explosive.GetTrigger();
			if (!trigger)
				continue;
			
			trigger.SetUser(m_User);
			trigger.UseTrigger();
		}
		
		m_aConnectedInventoryCharges.Clear();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Register switch when character picks it up
	override protected void ModeSwitch(EGadgetMode mode, IEntity charOwner)
	{
		if (charOwner != m_CharacterOwner)
		{
			if (Replication.IsServer() && m_CharacterOwner)
			{
				SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_CharacterOwner.GetCharacterController());
				if (charController)
					charController.ACE_Explosives_UnregisterDeadManSwitch(this);
			}
			
			ChimeraCharacter castedCharOwner = ChimeraCharacter.Cast(charOwner);
			if (Replication.IsServer() && castedCharOwner)
			{
				SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(castedCharOwner.GetCharacterController());
				if (charController)
					charController.ACE_Explosives_RegisterDeadManSwitch(this);
			}
			
			if (!castedCharOwner)
				RemoveAllChargesFromTheList();
		}
		
		super.ModeSwitch(mode, charOwner);
	}
}
