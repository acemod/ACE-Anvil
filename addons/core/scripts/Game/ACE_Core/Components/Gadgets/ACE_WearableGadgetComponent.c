//------------------------------------------------------------------------------------------------
class ACE_WearableGadgetComponentClass : SCR_ConsumableItemComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! For gadgets that are wearables. Use action will equip them.
class ACE_WearableGadgetComponent : SCR_ConsumableItemComponent
{
	//------------------------------------------------------------------------------------------------
	//! Update update visibility when gadget is moved from or to loadout slot
	override void OnParentSlotChanged(InventoryStorageSlot oldSlot, InventoryStorageSlot newSlot)
	{
		super.OnParentSlotChanged(oldSlot, newSlot);
		
		if (LoadoutSlotInfo.Cast(oldSlot) || LoadoutSlotInfo.Cast(newSlot))
			UpdateVisibility(m_iMode);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Show gadget when in loadout slot
	override void UpdateVisibility(EGadgetMode mode)
	{
		super.UpdateVisibility(mode);
		
		InventoryItemComponent itemComponent = InventoryItemComponent.Cast(GetOwner().FindComponent(InventoryItemComponent));
		if (!itemComponent)
			return;
		
		if (!LoadoutSlotInfo.Cast(itemComponent.GetParentSlot()))
			return;
			
		itemComponent.ShowOwner();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Cannot be held while worn
	override bool CanBeHeld()
	{
		InventoryItemComponent itemComponent = InventoryItemComponent.Cast(GetOwner().FindComponent(InventoryItemComponent));
		if (!itemComponent)
			return false;
		
		return !LoadoutSlotInfo.Cast(itemComponent.GetParentSlot());
	}
}
