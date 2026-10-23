//------------------------------------------------------------------------------------------------
modded class SCR_InventoryItemInfoUI
{
	//------------------------------------------------------------------------------------------------
	void ACE_Inventory_SetMetrics(InventoryItemComponent item)
	{
		if (!m_infoWidget)
			return;

		ACE_Inventory_ItemMetricsComponent metrics = ACE_Inventory_ItemMetricsComponent.Cast(m_infoWidget.FindHandler(ACE_Inventory_ItemMetricsComponent));
		if (metrics)
			metrics.UpdateMetrics(item);
	}
}
