//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_EGadgetScreenID, "m_eID")]
class ACE_Weather_KestrelAltitudeScreen : ACE_Weather_IKestrelDefaultDataScreen
{
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(float timeSlice)
	{
		int altitude = m_Kestrel.GetAltitude();
		m_wMainData.SetText(altitude.ToString());
	}
}
