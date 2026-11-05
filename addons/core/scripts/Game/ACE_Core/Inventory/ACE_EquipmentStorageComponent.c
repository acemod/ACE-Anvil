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
		
		InventoryItemComponent itemComponent = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
		if (!itemComponent)
			return false;
		
		InventoryStorageSlot sourceParentSlot = itemComponent.GetParentSlot();
		if (sourceParentSlot)
		{
			BaseInventoryStorageComponent sourceStorage = sourceParentSlot.GetStorage();
			if (!sourceStorage)
				return false;
			
			InventoryStorageManagerComponent sourceStorageManager = sourceStorage.GetHierarchyManager();
			if (!sourceStorageManager)
				return false;

			return sourceStorageManager.TryMoveItemToStorage(item, this, slotID);
		}
		
		SCR_InventoryStorageManagerComponent targetStorageManager = SCR_InventoryStorageManagerComponent.Cast(target.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!targetStorageManager)
			return false;

		return targetStorageManager.TryInsertItemInStorage(item, this, slotID);
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
