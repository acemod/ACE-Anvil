//------------------------------------------------------------------------------------------------
[EntityEditorProps(category: "GameScripted/UI/Inventory", description: "Inventory Item Info UI class")]
modded class SCR_InventoryDamageInfoUI : ScriptedWidgetComponent
{
	protected ImageWidget m_wACE_Medical_PneumothoraxIcon;
	protected TextWidget m_wACE_Medical_PneumothoraxText;
	
	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
		
		if (!w)
			return;
		
		m_wACE_Medical_PneumothoraxIcon = ImageWidget.Cast(w.FindAnyWidget("ACE_Medical_PneumothoraxInfo_icon"));
		m_wACE_Medical_PneumothoraxText = TextWidget.Cast(w.FindAnyWidget("ACE_Medical_PneumothoraxInfo_text"));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Show pneumothorax state
	void ACE_Medical_SetPneumothoraxStateVisible(bool visible, string text)
	{
		if (!m_wACE_Medical_PneumothoraxIcon || !m_wACE_Medical_PneumothoraxText)
			return;
		
		m_wACE_Medical_PneumothoraxIcon.SetVisible(visible);
		m_wACE_Medical_PneumothoraxText.SetVisible(visible);
		m_wACE_Medical_PneumothoraxText.SetText(text);
	}
}
