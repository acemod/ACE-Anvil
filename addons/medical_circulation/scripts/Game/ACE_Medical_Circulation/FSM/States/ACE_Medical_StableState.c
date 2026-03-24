//------------------------------------------------------------------------------------------------
class ACE_Medical_StableState : ACE_Medical_IVitalState
{
	//------------------------------------------------------------------------------------------------
	override protected void UpdateMetabolicAcidosisForState(ACE_Medical_CharacterContext context, float timeSlice)
	{
		CalculateMetabolicAcidosisRecovery(context, timeSlice);
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
