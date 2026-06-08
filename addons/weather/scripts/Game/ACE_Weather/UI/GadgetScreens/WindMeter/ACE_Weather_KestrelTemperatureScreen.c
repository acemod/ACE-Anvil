//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_EGadgetScreenID, "m_eID")]
class ACE_Weather_KestrelTemperatureScreen : ACE_Weather_IKestrelDefaultDataScreen
{
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(float timeSlice)
	{
		float temperature = m_Kestrel.GetTemperature();
		m_wMainData.SetText(temperature.ToString(lenDec: 1));
	}
}
