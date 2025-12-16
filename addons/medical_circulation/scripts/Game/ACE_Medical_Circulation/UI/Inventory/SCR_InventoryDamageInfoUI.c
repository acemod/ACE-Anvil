//------------------------------------------------------------------------------------------------
[EntityEditorProps(category: "GameScripted/UI/Inventory", description: "Inventory Item Info UI class")]
modded class SCR_InventoryDamageInfoUI : ScriptedWidgetComponent
{
	//------------------------------------------------------------------------------------------------
	//! Insert blood state in bleeding widget
	void ACE_Medical_SetBloodStateVisible(bool visible, string text)
	{
		if (!visible || !m_wBleedingIconWidget || !m_wBleedingTextWidget)
			return;
		
		string bleedingText;
		if (m_wBleedingTextWidget.IsVisible())
			text += "\n" + m_wBleedingTextWidget.GetText();
		
		m_wBleedingIconWidget.SetVisible(visible);
		m_wBleedingTextWidget.SetVisible(visible);
		m_wBleedingTextWidget.SetText(text);
	}
}
