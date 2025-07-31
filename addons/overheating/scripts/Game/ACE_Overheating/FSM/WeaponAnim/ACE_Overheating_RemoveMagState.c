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
			
			bool result;
			
			// Trying to move magazine to storage is broken in MP
			// See (1) in https://feedback.bistudio.com/T193647
			/*
			BaseInventoryStorageComponent suitableStorage = context.m_pInventoryManager.FindStorageForItem(currentMag);
			if (suitableStorage)
				result = context.m_pInventoryManager.TryMoveItemToStorage(currentMag, suitableStorage);
			else
			*/
				result = context.m_pInventoryManager.TryRemoveItemFromStorage(currentMag, magStorage);
			
			context.m_bIsStartRemovingMag = result;
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
		
		if (context.m_bIsStartRemovingMag && context.IsReloading())
			context.m_bIsStartRemovingMag = false;
	}
}
