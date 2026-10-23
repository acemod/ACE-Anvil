//------------------------------------------------------------------------------------------------
modded class SCR_InventoryMenuUI
{
	protected float m_fACE_Inventory_MetricsRefreshElapsed;

	//------------------------------------------------------------------------------------------------
	override void OnMenuUpdate(float tDelta)
	{
		super.OnMenuUpdate(tDelta);
		m_fACE_Inventory_MetricsRefreshElapsed += tDelta;
		if (m_fACE_Inventory_MetricsRefreshElapsed < 0.2)
			return;
		m_fACE_Inventory_MetricsRefreshElapsed = 0;
		ACE_Inventory_RefreshFocusedMetrics();
	}

	//------------------------------------------------------------------------------------------------
	protected void ACE_Inventory_RefreshFocusedMetrics()
	{
		if (!m_pItemInfo)
			return;

		SCR_InventorySlotUI focused = GetCurrentlyFocusedSlot();
		InventoryItemComponent item;
		if (focused)
			item = focused.GetInventoryItemComponent();

		m_pItemInfo.ACE_Inventory_SetMetrics(item);
	}

	//------------------------------------------------------------------------------------------------
	override void ShowItemInfo(string sName = "", string sDescr = "", float sWeight = 0.0, SCR_InventoryUIInfo uiInfo = null)
	{
		super.ShowItemInfo(sName, sDescr, sWeight, uiInfo);
		ACE_Inventory_RefreshFocusedMetrics();
	}
}
