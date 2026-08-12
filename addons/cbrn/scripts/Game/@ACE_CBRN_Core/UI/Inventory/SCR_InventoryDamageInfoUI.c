//------------------------------------------------------------------------------------------------
[EntityEditorProps(category: "GameScripted/UI/Inventory", description: "Inventory Item Info UI class")]
modded class SCR_InventoryDamageInfoUI : ScriptedWidgetComponent
{
	protected ImageWidget m_wACE_CBRN_ParalysisIcon;
	protected TextWidget m_wACE_CBRN_ParalysisText;
	
	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
		
		if (!w)
			return;
		
		m_wACE_CBRN_ParalysisIcon = ImageWidget.Cast(w.FindAnyWidget("ACE_CBRN_ParalysisInfo_icon"));
		m_wACE_CBRN_ParalysisText = TextWidget.Cast(w.FindAnyWidget("ACE_CBRN_ParalysisInfo_text"));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Show paralysis state
	void ACE_CBRN_SetParalysisStateVisible(bool visible)
	{
		if (!m_wACE_CBRN_ParalysisIcon || !m_wACE_CBRN_ParalysisText)
			return;
		
		m_wACE_CBRN_ParalysisIcon.SetVisible(visible);
		m_wACE_CBRN_ParalysisText.SetVisible(visible);
	}
}
