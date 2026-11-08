//------------------------------------------------------------------------------------------------
class ACE_Medical_CriticalState : ACE_Medical_StableState
{
	//------------------------------------------------------------------------------------------------
	//! Force uncon
	override void OnEnter(ACE_Medical_CharacterContext context)
	{
		super.OnEnter(context);
		// Force uncon
		context.m_pDamageManager.GetResilienceHitZone().SetHealth(0);
		context.m_pDamageManager.UpdateConsciousness();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Add hypovolemic shock induced tachycardia
	override protected float ComputeBaseTargetHeartRate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float bloodVolumeRatio = context.m_pBloodHitZone.GetHealthScaled();
		if (bloodVolumeRatio < context.m_pBloodHitZone.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_3_HEMORRHAGE))
			return context.m_pVitals.GetHeartRate() * 14.3 * Math.Lerp(0.5, 1, bloodVolumeRatio) / Math.Max(6, context.m_pVitals.GetMeanArterialPressure());
		
		return super.ComputeBaseTargetHeartRate(context, timeSlice);
	}
}
