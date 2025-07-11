//------------------------------------------------------------------------------------------------
class ACE_Medical_CriticalState : ACE_Medical_StableState
{
	//------------------------------------------------------------------------------------------------
	//! Add hypovolemic shock induced tachycardia
	override protected float ComputeBaseTargetHeartRate(ACE_Medical_VitalStates_CharacterContext context, float timeSlice)
	{
		float bloodVolumeRatio = context.m_pBloodHitZone.GetHealthScaled();
		if (bloodVolumeRatio < context.m_pBloodHitZone.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_3_HEMORRHAGE))
			return context.m_pComponent.GetHeartRate() * 14.3 * Math.Lerp(0.5, 1, bloodVolumeRatio) / Math.Max(6, context.m_pComponent.GetMeanArterialPressure());
		
		return s_pCirculationSettings.m_fDefaultHeartRateBPM;
	}
}
