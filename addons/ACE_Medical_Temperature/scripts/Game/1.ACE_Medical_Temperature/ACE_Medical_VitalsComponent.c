modded class ACE_Medical_VitalsComponent : ACE_BaseComponent
{	
	
	//Kelvin = 273 + degrees celsius
	float m_fInsulationScore;//Insulation factor of clothing - 0.03 = 3% of outdoor heat enters/escapes
	int m_iHeatPackCount;
	float m_fWetness;
	//Returns the core temperature of player in celsius
	
	float GetInsulation(){
		return m_fInsulationScore;
	}
	
	override void Reset()
	{
		super.Reset();
		m_fWetness=0;
		m_iHeatPackCount=0;
		ACE_Medical_Temperature_Settings settings = ACE_SettingsHelperT<ACE_Medical_Temperature_Settings>.GetModSettings();
		if (settings)
		{
			m_fCoreTemperature=settings.m_fDefaultCoreTemperature;
			m_fInsulationScore=settings.m_fInsulationScore;
		}
	}
}