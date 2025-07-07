//------------------------------------------------------------------------------------------------
class ACE_Overheating_RemoveMagState : ACE_FSM_IState<ACE_Overheating_WeaponAnimContext>
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Overheating_WeaponAnimContext context)
	{
		context.m_pCharController.OverrideMaxSpeed(0.25);
		
		BaseMagazineComponent currentMagComponent = context.m_pWeapon.GetCurrentMagazine();
		if (currentMagComponent)
		{
			IEntity currentMag = currentMagComponent.GetOwner();
			InventoryItemComponent magInventory = InventoryItemComponent.Cast(currentMag.FindComponent(InventoryItemComponent));
			BaseInventoryStorageComponent magStorage = magInventory.GetParentSlot().GetStorage();
			
			BaseInventoryStorageComponent suitableStorage = context.m_pInventoryManager.FindStorageForItem(currentMag);
			if (suitableStorage)
				context.m_bIsStartRemovingMag = context.m_pInventoryManager.TryMoveItemToStorage(currentMag, suitableStorage);
			else
				context.m_bIsStartRemovingMag = context.m_pInventoryManager.TryRemoveItemFromStorage(currentMag, magStorage);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnExit(ACE_Overheating_WeaponAnimContext context)
	{
		context.m_pCharController.OverrideMaxSpeed(1);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Overheating_WeaponAnimContext context, float timeSlice)
	{
		context.m_pCharController.SetWeaponADS(false);
	}
}
