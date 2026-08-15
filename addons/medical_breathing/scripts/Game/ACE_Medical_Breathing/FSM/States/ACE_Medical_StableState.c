//------------------------------------------------------------------------------------------------
modded class ACE_Medical_StableState : ACE_Medical_IVitalState
{
	//------------------------------------------------------------------------------------------------
	//! TO DO: Add adjustments to SpO2
	override protected float ComputeBaseTargetHeartRate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float rate = super.ComputeBaseTargetHeartRate(context, timeSlice);
		
		float tachypnea = (context.m_pVitals.GetRespiratoryRate() - s_pBreathingSettings.m_fDefaultRespiratoryRateBPM);
		if (tachypnea <= 0)
			return rate;
		
		return rate + 2 * tachypnea;
	}
}
