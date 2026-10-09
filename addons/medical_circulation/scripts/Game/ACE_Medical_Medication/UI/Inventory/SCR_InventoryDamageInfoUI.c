//------------------------------------------------------------------------------------------------
[EntityEditorProps(category: "GameScripted/UI/Inventory", description: "Inventory Item Info UI class")]
modded class SCR_InventoryDamageInfoUI : ScriptedWidgetComponent
{
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_SetMedicationVisible(bool visible, string text)
	{
		if (!m_wMorphineIconWidget || !m_wMorphineTextWidget)
			return;
		
		m_wMorphineIconWidget.LoadImageFromSet(0, m_sMedicalIconsImageSet, m_sMorphineIcon);
		m_wMorphineIconWidget.SetVisible(visible);
		
		m_wMorphineTextWidget.SetText(text);
		m_wMorphineTextWidget.SetVisible(visible);
	}
}
