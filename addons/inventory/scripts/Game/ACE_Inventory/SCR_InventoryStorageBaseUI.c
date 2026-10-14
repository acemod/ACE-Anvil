modded class SCR_InventoryStorageBaseUI
{
	//------------------------------------------------------------------------------------------------
	bool ACE_Inventory_GetStack(SCR_InventorySlotUI selected, out array<IEntity> items)
	{
		items = {};
		int selectedId = -1;
		if (selected)
			selectedId = GetSlotId(selected);
		if (selectedId < 0)
		{
			Print("[ACE Inventory] Selected slot is no longer in the source UI.", LogLevel.WARNING);
			return false;
		}

		if (!selected.IsStacked())
		{
			InventoryItemComponent component = selected.GetInventoryItemComponent();
			if (!component)
			{
				Print("[ACE Inventory] Selected slot has no inventory item.", LogLevel.WARNING);
				return false;
			}

			items.Insert(component.GetOwner());
			return true;
		}

		array<IEntity> visibleItems = {};
		GetAllItems(visibleItems);
		foreach (IEntity item : visibleItems)
		{
			if (!item)
				continue;

			InventoryItemComponent component = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
			if (component && FindItem(component) == selectedId)
				items.Insert(item);
		}

		if (items.Count() != selected.GetStackNumber())
		{
			PrintFormat("[ACE Inventory] Stack changed: UI=%1, snapshot=%2. Transfer cancelled.", selected.GetStackNumber(), items.Count(), level: LogLevel.WARNING);
			items.Clear();
			return false;
		}

		return true;
	}
}
