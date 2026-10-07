//------------------------------------------------------------------------------------------------
modded class SCR_InventoryMenuUI : ChimeraMenuBase
{
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_SetSplintStateVisible(bool visible)
	{
		if (m_pDamageInfo)
			m_pDamageInfo.ACE_Medical_SetSplintStateVisible(visible);
	}
}
