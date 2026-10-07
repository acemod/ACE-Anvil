//------------------------------------------------------------------------------------------------
modded class SCR_InventoryHitZonePointUI : ScriptedWidgetComponent
{
	//------------------------------------------------------------------------------------------------
	override protected void ShowHitZoneInfo(bool show, bool virtualHZ = false)
	{
		super.ShowHitZoneInfo(show, virtualHZ);
		
		if (!show || virtualHZ || !m_pParentContainer || !m_pParentContainer.m_pCharDmgManager)
			return;
		
		SCR_InventoryMenuUI inventoryMenu = m_pParentContainer.GetInventoryHandler();
		if (!inventoryMenu)
			return;
		
		inventoryMenu.ACE_Medical_SetSplintStateVisible(m_pParentContainer.m_pCharDmgManager.ACE_Medical_IsGroupSplinted(m_pParentContainer.GetHitZoneGroup()));
	}
}
