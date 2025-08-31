//------------------------------------------------------------------------------------------------
modded class ACE_Medical_VitalStateThresholdTransition : ACE_FSM_ITransition<ACE_Medical_CharacterContext>
{
	//------------------------------------------------------------------------------------------------
	//! Evaluate SpO2
	override bool ShouldBePerformed(ACE_Medical_CharacterContext context, float timeSlice)
	{
		if (super.ShouldBePerformed(context, timeSlice))
			return true;
		
		return context.m_pVitals.GetSpO2() < m_pThresholds.m_fSpO2;
	}
}
