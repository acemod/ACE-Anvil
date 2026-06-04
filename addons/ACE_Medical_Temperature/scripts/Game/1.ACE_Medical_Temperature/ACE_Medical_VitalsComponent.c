modded class ACE_Medical_VitalsComponent : ACE_BaseComponent
{	
	
	//Kelvin = 273 + degrees celsius
 	float m_fCoreTemperature;//Core temperature of patient, Kelvin
	float m_fInsulationScore=0.03;//Insulation factor of clothing - 0.03 = 3% of outdoor heat enters/escapes
	int m_iHeatPackCount=0;
	float m_fWetness=0;
	//Returns the core temperature of player in celsius
	float GetTemperature()
	{
		return m_fCoreTemperature;
	}
	
	void SetTemperature(float m_fNewTemperature){
		m_fCoreTemperature = m_fNewTemperature;
	}
	
	float GetInsulation(){
		return m_fInsulationScore;
	}
	
	override void Reset()
	{
		super.Reset();
		ACE_Medical_Temperature_Settings settings = ACE_SettingsHelperT<ACE_Medical_Temperature_Settings>.GetModSettings();
		if (settings)
		{
			m_fCoreTemperature=settings.m_fDefaultCoreTemperature;
		}
	}
}