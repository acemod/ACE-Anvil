//------------------------------------------------------------------------------------------------
modded class SCR_InventoryMenuUI
{
	//------------------------------------------------------------------------------------------------
	override void ShowItemInfo(string sName = "", string sDescr = "", float sWeight = 0.0, SCR_InventoryUIInfo uiInfo = null)
	{
		super.ShowItemInfo(sName, sDescr, sWeight, uiInfo);

		if (!m_pItemInfo)
			return;

		SCR_InventorySlotUI focused = GetCurrentlyFocusedSlot();
		InventoryItemComponent item;
		if (focused)
			item = focused.GetInventoryItemComponent();

		m_pItemInfo.ACE_SetInventoryMetrics(item);
	}
}
