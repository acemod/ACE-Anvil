//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_EGadgetScreenID, "m_eID")]
class ACE_Weather_IKestrelDefaultDataScreen : ACE_Weather_IKestrelDataScreen
{
	protected TextWidget m_wMainData;
	protected TextWidget m_wSecondaryData;
	
	//------------------------------------------------------------------------------------------------
	override void OnOpen(Widget root)
	{
		super.OnOpen(root);
		Widget defaultWidget = root.FindAnyWidget("Default");
		defaultWidget.SetVisible(true);
		m_wMainData = TextWidget.Cast(defaultWidget.FindAnyWidget("MainData"));
		m_wMainData.SetVisible(true);
		m_wSecondaryData = TextWidget.Cast(defaultWidget.FindAnyWidget("SecondaryData"));
		m_wSecondaryData.SetVisible(HasSecondaryData());
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool HasSecondaryData()
	{
		return false;
	}
}
