//------------------------------------------------------------------------------------------------
modded class SCR_InventoryStorageBaseUI
{
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
