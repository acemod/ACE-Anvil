//------------------------------------------------------------------------------------------------
class ACE_Medical_StableState : ACE_Medical_IVitalState
{
	//------------------------------------------------------------------------------------------------
	override protected float ComputeHeartRate(ACE_Medical_VitalStates_CharacterContext context, float timeSlice)
	{
		float targetHeartRate = ComputeTargetHeartRate(context, timeSlice);
		float prevHeartRate = context.m_pComponent.GetHeartRate();
		float progress = Math.Min(0.5 * timeSlice, 1);
		return Math.Lerp(prevHeartRate, targetHeartRate, progress);
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeTargetHeartRate(ACE_Medical_VitalStates_CharacterContext context, float timeSlice)
	{
		float target = ComputeBaseTargetHeartRate(context, timeSlice);
		target = Math.Max(target, s_pCirculationSettings.m_fDefaultHeartRateBPM + 50 * context.m_pDamageManager.ACE_Medical_GetPainIntensity());
		/***** target += context.m_pComponent.GetHeartRateMedicationAdjustment(); *****/
		// TO DO: SpO2-based adjustments
		
		return Math.Max(0, target);
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeBaseTargetHeartRate(ACE_Medical_VitalStates_CharacterContext context, float timeSlice)
	{
		return s_pCirculationSettings.m_fDefaultHeartRateBPM;
	}
}
