//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_EGadgetScreenID, "m_eID")]
class ACE_Weather_IKestrelRefHeadingSetScreen : ACE_Weather_IKestrelRefHeadingScreen
{
	[Attribute(uiwidget: UIWidgets.EditBoxMultiline)]
	protected string m_sInstructions;
	
	//------------------------------------------------------------------------------------------------
	override void OnOpen(Widget root)
	{
		super.OnOpen(root);
		m_wOptions.SetVisible(false);
		m_wInstructions.SetVisible(true);
		m_wInstructions.SetText(m_sInstructions);
	}
}
