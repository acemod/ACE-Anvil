modded class ACE_Medical_VitalStateThresholdTransition : ACE_FSM_ITransition<ACE_Medical_CharacterContext>{
	override bool ShouldBePerformed(ACE_Medical_CharacterContext context, float timeSlice)
	{
		if (super.ShouldBePerformed(context, timeSlice))
			return true;
		
		return context.m_pVitals.GetTemperature() < m_pThresholds.m_fTemperatureThreshold;
	}

}