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
		BaseInventoryStorageComponent source = GetCurrentNavigationStorage();
		if (source)
		{
			InventoryItemComponent representative = selected.GetInventoryItemComponent();
			if (!representative)
			{
				Print("[ACE Inventory] Selected stack has no inventory item.", LogLevel.WARNING);
				return false;
			}

			// Enumerate physical contents, not an arsenal catalog on the same vehicle.
			ACE_Inventory_Stack.GetGroup(source, representative.GetOwner(), visibleItems);
		}
		else
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

	//------------------------------------------------------------------------------------------------
	override void Init()
	{
		super.Init();
		ACE_Inventory_UpdateMetrics();
	}

	//------------------------------------------------------------------------------------------------
	override void Refresh()
	{
		super.Refresh();
		ACE_Inventory_UpdateMetrics();
	}

	//------------------------------------------------------------------------------------------------
	override void UpdateVolumePercentage(float percentage, bool previewOnly = false)
	{
		super.UpdateVolumePercentage(percentage, previewOnly);
		if (!previewOnly)
			ACE_Inventory_UpdateMetrics();
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_UpdateMetrics()
	{
		Widget root = GetRootWidget();
		if (!root)
			return;

		ACE_Inventory_ContainerMetricsComponent metrics = ACE_Inventory_ContainerMetricsComponent.Cast(root.FindHandler(ACE_Inventory_ContainerMetricsComponent));
		if (metrics)
			metrics.UpdateMetrics(GetStorage());
	}
}
