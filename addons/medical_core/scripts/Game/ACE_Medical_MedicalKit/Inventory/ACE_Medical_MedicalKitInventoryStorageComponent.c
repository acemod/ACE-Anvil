//------------------------------------------------------------------------------------------------
class ACE_Medical_MedicalKitInventoryStorageComponentClass : SCR_UniversalInventoryStorageComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_MedicalKitInventoryStorageComponent : SCR_UniversalInventoryStorageComponent
{
	//------------------------------------------------------------------------------------------------
	//! Only allow medical items
	override bool CanStoreItem(IEntity item, int slotID)
	{
		if (!super.CanStoreItem(item, slotID))
			return false;
		
		return item.FindComponent(SCR_ConsumableItemComponent);
	}
}
