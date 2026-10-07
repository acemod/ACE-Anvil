//------------------------------------------------------------------------------------------------
modded class SCR_InventoryDamageInfoUI : ScriptedWidgetComponent
{
	protected ImageWidget m_wACE_Medical_SplintIcon;
	protected TextWidget m_wACE_Medical_SplintText;
	
	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
		
		if (!w)
			return;
		
		m_wACE_Medical_SplintIcon = ImageWidget.Cast(w.FindAnyWidget("ACE_Medical_SplintInfo_icon"));
		m_wACE_Medical_SplintText = TextWidget.Cast(w.FindAnyWidget("ACE_Medical_SplintInfo_text"));
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_SetSplintStateVisible(bool visible)
	{
		if (!m_wACE_Medical_SplintIcon || !m_wACE_Medical_SplintText)
			return;
		
		m_wACE_Medical_SplintIcon.LoadImageFromSet(0, m_sMedicalIconsImageSet, m_sFractureIcon);
		m_wACE_Medical_SplintIcon.SetVisible(visible);
		m_wACE_Medical_SplintText.SetText("#ACE_Medical-DamageInfo_SplintApplied");
		m_wACE_Medical_SplintText.SetVisible(visible);
	}
}
