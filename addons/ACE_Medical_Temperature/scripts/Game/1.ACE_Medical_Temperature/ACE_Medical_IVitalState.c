modded class ACE_Medical_IVitalState : ACE_FSM_IState<ACE_Medical_CharacterContext>{
	protected static ACE_Medical_Temperature_Settings s_pTemperatureSettings;
	protected static float s_fDefaultCoreTemperature;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_IVitalState(ACE_FSM_EStateID id)
	{
		if (!s_pTemperatureSettings)
		{
			s_pTemperatureSettings = ACE_SettingsHelperT<ACE_Medical_Temperature_Settings>.GetModSettings();
			s_fDefaultCoreTemperature = s_pTemperatureSettings.m_fDefaultCoreTemperature;
		}
	}
	
	override void OnUpdate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		updateTemperature(context,timeSlice);
	}
	protected void updateTemperature(ACE_Medical_CharacterContext context, float timeSlice)
	{
		
		//Get the current core temperature
		float m_fCurrentCoreTemperature=context.m_pVitals.m_fCoreTemperature;
		//float outdoorTemperature = m_fAmbientTemperature
		//Start with the ambient temperature at sea level
		float m_fFinalOutdoorTemperature = s_pTemperatureSettings.m_fAmbientTemperature;
		//Adjust temperature by -6.5c per KM
		//m_fFinalOutdoorTemperature = 
		//Calculate how different the outdoor air is to core temperature
		float m_fOutdoorTemperatureDiff = m_fFinalOutdoorTemperature-context.m_pVitals.m_fCoreTemperature;
		//Reduce the impact of outside temperature by insulation
		float m_fCoreTemperatureChange = m_fOutdoorTemperatureDiff*context.m_pVitals.m_fInsulationFactor;
		//Apply all the changes to core temperature
		context.m_pVitals.SetTemperature(m_fCurrentCoreTemperature + m_fCoreTemperatureChange*timeSlice);
		
		//Todo: implement body heat system
		
		
	}
}