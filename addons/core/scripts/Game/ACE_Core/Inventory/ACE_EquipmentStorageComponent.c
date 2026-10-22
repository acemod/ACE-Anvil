//------------------------------------------------------------------------------------------------
class ACE_EquipmentStorageComponentClass : SCR_EquipmentStorageComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_EquipmentStorageComponent : SCR_EquipmentStorageComponent
{
	//------------------------------------------------------------------------------------------------
	bool AddItemToSlot(IEntity target, IEntity item, ACE_EEquipementSlot slotID)
	{
		if (!target || !item)
			return false;
		
		RplComponent rpl = RplComponent.Cast(target.FindComponent(RplComponent));
		if (rpl && rpl.IsProxy())
			return false;
		
		SCR_InventoryStorageManagerComponent storageManager = SCR_InventoryStorageManagerComponent.Cast(GetOwner().FindComponent(SCR_InventoryStorageManagerComponent));
		if (!storageManager)
			return false;
		
		if (storageManager.TryMoveItemToStorage(item, this, slotID))
			return true;
		
		if (storageManager.TryInsertItemInStorage(item, this, slotID))
			return true;
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsSlotOccupied(ACE_EEquipementSlot slotID)
	{
		return GetSlot(slotID).GetStorage().Get(slotID);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected bool ShouldPreviewAttachedItems()
	{
		return true;
	}
}
