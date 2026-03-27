//------------------------------------------------------------------------------------------------
modded class SCR_InventoryMenuUI : ChimeraMenuBase
{
	//------------------------------------------------------------------------------------------------
	//! Show blood loss widget for Class I already
	override void OnDamageStateChanged()
	{
		super.OnDamageStateChanged();
		
		if (m_bleedingHandlerGlobal)
			m_bleedingHandlerGlobal.GetRootWidget().SetVisible(m_CharDamageManager.GetBloodHitZone().GetDamageState() > ACE_Medical_EBloodState.NORMAL);
	}
}
