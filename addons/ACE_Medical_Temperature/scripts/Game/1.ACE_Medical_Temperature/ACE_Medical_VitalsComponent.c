modded class ACE_Medical_VitalsComponent : ACE_BaseComponent
{
	//Kelvin = 273 + degrees celsius
 	float m_fCoreTemperature;//Core temperature of patient, Kelvin
	float m_fInsulationFactor=0.001;//Insulation factor of clothing - 0.03 = 3% of core heat escapes
	//Returns the core temperature of player in celsius
	float GetTemperature()
	{
		return m_fCoreTemperature;
	}
	void SetTemperature(float m_fNewTemperature){
		m_fCoreTemperature = m_fNewTemperature;
	}
	override void Reset()
	{
		super.Reset();
		ACE_Medical_Temperature_Settings settings = ACE_SettingsHelperT<ACE_Medical_Temperature_Settings>.GetModSettings();
		if (settings)
		{
			m_fCoreTemperature=settings.m_fDefaultCoreTemperature
		}
	}
}