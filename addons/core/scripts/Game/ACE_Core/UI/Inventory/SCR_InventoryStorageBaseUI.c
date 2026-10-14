//------------------------------------------------------------------------------------------------
modded class SCR_InventoryStorageBaseUI
{
	//------------------------------------------------------------------------------------------------
	override void Init()
	{
		super.Init();
		ACE_UpdateInventoryMetrics();
	}

	//------------------------------------------------------------------------------------------------
	override void Refresh()
	{
		super.Refresh();
		ACE_UpdateInventoryMetrics();
	}

	//------------------------------------------------------------------------------------------------
	override void UpdateVolumePercentage(float percentage, bool previewOnly = false)
	{
		super.UpdateVolumePercentage(percentage, previewOnly);
		if (!previewOnly)
			ACE_UpdateInventoryMetrics();
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_UpdateInventoryMetrics()
	{
		Widget root = GetRootWidget();
		if (!root)
			return;

		ACE_InventoryContainerMetricsComponent metrics = ACE_InventoryContainerMetricsComponent.Cast(root.FindHandler(ACE_InventoryContainerMetricsComponent));
		if (metrics)
			metrics.UpdateMetrics(GetStorage());
	}
}
