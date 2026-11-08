//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_EGadgetScreenID, "m_eID")]
class ACE_Weather_KestrelWindSpeedScreen : ACE_Weather_IKestrelDefaultDataScreen
{
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(float timeSlice)
	{
		float speed = m_Kestrel.GetMeasuredWindSpeed();
		m_wMainData.SetText(speed.ToString(-1, 1));
	}
}
