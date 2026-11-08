//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_EGadgetScreenID, "m_eID")]
class ACE_Weather_KestrelHomeScreen : ACE_Weather_IKestrelScreen
{
	protected TextWidget m_wDayOfTheWeek;
	protected TextWidget m_wDate;
	protected TextWidget m_wTime;
	
	//------------------------------------------------------------------------------------------------
	override void OnOpen(Widget root)
	{
		super.OnOpen(root);
		
		Widget displayHome = root.FindAnyWidget("DisplayHome");
		displayHome.SetVisible(true);
		
		m_wDayOfTheWeek = TextWidget.Cast(displayHome.FindAnyWidget("DayOfTheWeek"));
		m_wDate = TextWidget.Cast(displayHome.FindAnyWidget("Date"));
		m_wTime = TextWidget.Cast(displayHome.FindAnyWidget("Time"));
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(float timeSlice)
	{
		m_wDayOfTheWeek.SetText(m_Kestrel.GetWeekDay());
		m_wDate.SetText(m_Kestrel.GetDate());
		m_wTime.SetText(m_Kestrel.GetTime());
	}
}
