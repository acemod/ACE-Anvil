//------------------------------------------------------------------------------------------------
class ACE_Medical_StableState : ACE_Medical_IVitalState
{
	//------------------------------------------------------------------------------------------------
	override protected void UpdateMetabolicAcidosisForState(ACE_Medical_CharacterContext context, float timeSlice)
	{
		CalculateMetabolicAcidosisRecovery(context, timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CalculateMetabolicAcidosisRecovery(ACE_Medical_CharacterContext context, float timeSlice)
	{
		ACE_Medical_Acidosis.CalculateMetabolicAcidosisRecovery(context, timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Updates blood pH based on blood volume and time (worsening = pH down, recovery = pH up)
	//! @param isIncreasing True for critical/cardiac arrest (acidosis worsens, pH decreases), false for recovery
	protected void UpdateMetabolicAcidosis(ACE_Medical_CharacterContext context, float timeSlice, bool isIncreasing)
	{
		ACE_Medical_Acidosis.UpdateMetabolicAcidosis(context, timeSlice, isIncreasing);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected float ComputeHeartRate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float targetHeartRate = ComputeTargetHeartRate(context, timeSlice);
		float prevHeartRate = context.m_pVitals.GetHeartRate();
		float progress = Math.Min(0.5 * timeSlice, 1);
		return Math.Lerp(prevHeartRate, targetHeartRate, progress);
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeTargetHeartRate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float target = ComputeBaseTargetHeartRate(context, timeSlice);
		target = Math.Max(target, s_pCirculationSettings.m_fDefaultHeartRateBPM + 50 * context.m_pDamageManager.ACE_Medical_GetPainIntensity());
		target += context.m_pVitals.GetHeartRateMedicationAdjustment();
		// TO DO: SpO2-based adjustments
		
		return Math.Max(0, target);
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeBaseTargetHeartRate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return s_pCirculationSettings.m_fDefaultHeartRateBPM;
	}
}
