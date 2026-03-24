//------------------------------------------------------------------------------------------------
class ACE_Medical_StableState : ACE_Medical_IVitalState
{
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
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
		float progress = Math.Clamp(0.5 * timeSlice, 0.0, 1.0);
		return Math.Lerp(context.m_pVitals.GetHeartRate(), ComputeTargetHeartRate(context, timeSlice), progress);
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeTargetHeartRate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float painAdjustedHR = s_pCirculationSettings.m_fDefaultHeartRateBPM + 50 * context.m_pDamageManager.ACE_Medical_GetPainIntensity();
		float target = Math.Max(ComputeBaseTargetHeartRate(context, timeSlice), painAdjustedHR);
		target += context.m_pVitals.GetHeartRateMedicationAdjustment();
		// TO DO: SpO2-based adjustments
		
		return Math.Max(0.0, target);
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeBaseTargetHeartRate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return s_pCirculationSettings.m_fDefaultHeartRateBPM;
	}
}
