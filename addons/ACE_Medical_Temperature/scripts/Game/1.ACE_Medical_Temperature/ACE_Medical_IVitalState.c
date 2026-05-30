modded class ACE_Medical_IVitalState : ACE_FSM_IState<ACE_Medical_CharacterContext>{
	override void OnUpdate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
	}
	protected void updateTemperature(ACE_Medical_CharacterContext context, float timeSlice){
		if (!m_bEnableTemperature){
			return;
		}
		//Get the current core temperature
		float m_fCurrentCoreTemperature=context.m_fCoreTemperature;
		//float outdoorTemperature = m_fAmbientTemperature
		//TODO Calculate the final outdoor temperature (placeholder)
		float m_fFinalOutdoorTemperature = m_fAmbientTemperature;
		
		//Calculate how different the outdoor air is to core temperature
		float m_fOutdoorTemperatureDiff = m_fFinalTemperature-context.m_fCoreTemperature;
		//Reduce by how much insulation the player is wearing
		float m_fOutdoorTemperatureChange=context.m_fInsulationFactor * m_fTemperatureDiff;
		
		float m_fCoreTemperatureChange = m_fOutdoorTemperatureChange;
		
		context.m_fCoreTemperature=m_fCurrentCoreTemperature + m_fCoreTemperatureChange*timeSlice;
		
		//float m_fBodyHeat = m_fComplicatedKATBAShit;
		
		
	}
}