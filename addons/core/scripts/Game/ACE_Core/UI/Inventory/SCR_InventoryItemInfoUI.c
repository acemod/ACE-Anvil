//------------------------------------------------------------------------------------------------
modded class SCR_InventoryItemInfoUI
{
	//------------------------------------------------------------------------------------------------
	void ACE_SetInventoryMetrics(InventoryItemComponent item)
	{
		if (!m_infoWidget)
			return;

		ACE_InventoryItemMetricsComponent metrics = ACE_InventoryItemMetricsComponent.Cast(m_infoWidget.FindHandler(ACE_InventoryItemMetricsComponent));
		if (metrics)
			metrics.UpdateMetrics(item);
	}
}
