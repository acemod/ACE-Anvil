//------------------------------------------------------------------------------------------------
class ACE_Explosives_ArmInventoryExplosivesAction : SCR_RemoteDetonatorAction
{
	protected ref ACE_NotConnectedToDetonatorItemPredicate m_ItemSearchPredicate = new ACE_NotConnectedToDetonatorItemPredicate();
	protected ACE_Explosives_DeadManSwitchGadgetComponent m_DeadManSwitch;
	
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		m_ItemSearchPredicate.m_DetonatorComponent = m_DetonatorComp;
		m_DeadManSwitch = ACE_Explosives_DeadManSwitchGadgetComponent.Cast(m_DetonatorComp);
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!m_DeadManSwitch)
			return;
		
		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(pUserEntity.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventoryManager)
			return;
		
		array<IEntity> explosives = {};
		inventoryManager.FindItems(explosives, m_ItemSearchPredicate);		
		m_DeadManSwitch.ConnectToInventoryCharges(explosives);
		
		SoundComponent soundComp = SoundComponent.Cast(pOwnerEntity.FindComponent(SoundComponent));
		if (soundComp)
			soundComp.SoundEvent(SCR_SoundEvent.SOUND_DETONATOR_DISCONNECT_WIRES);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventoryManager)
			return false;
		
		if (!inventoryManager.FindItem(m_ItemSearchPredicate))
		{
			SetCannotPerformReason("#AR-UserActionUnavailable");
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBroadcastScript()
	{
		return true;
	}
}
