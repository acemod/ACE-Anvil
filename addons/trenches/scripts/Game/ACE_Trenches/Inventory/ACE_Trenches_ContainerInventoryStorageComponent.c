//------------------------------------------------------------------------------------------------
class ACE_Trenches_ContainerInventoryStorageComponentClass : SCR_UniversalInventoryStorageComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_Trenches_ContainerInventoryStorageComponent : SCR_UniversalInventoryStorageComponent
{
	//------------------------------------------------------------------------------------------------
	//! Only allow storing multipart deployable items
	override bool CanStoreItem(IEntity item, int slotID)
	{
		if (!super.CanStoreItem(item, slotID))
			return false;
		
		return item.FindComponent(SCR_MultiPartDeployableItemComponent);
	}
}
