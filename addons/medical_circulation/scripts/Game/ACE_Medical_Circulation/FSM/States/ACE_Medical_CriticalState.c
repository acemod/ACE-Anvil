//------------------------------------------------------------------------------------------------
class ACE_Medical_CriticalState : ACE_Medical_StableState
{
	static const float HR_MAX_ACIDOSIS_BPM = 30.0; // [bpm] HR at 100% metabolic acidosis
	
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
	override void OnUpdate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		// In critical state, acidosis increases (worsens) rather than recovers
		UpdateMetabolicAcidosis(context, timeSlice, true);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Override to prevent recovery calculation in critical state
	//! Acidosis should only increase in critical state, so we skip the recovery step from parent
	override protected void CalculateMetabolicAcidosisRecovery(ACE_Medical_CharacterContext context, float timeSlice)
	{
		// Intentionally empty - critical state uses increasing acidosis instead
		// This prevents the parent StableState from decreasing acidosis, which would then be immediately undone
	}
	
	//------------------------------------------------------------------------------------------------
	override protected float ComputeBaseTargetHeartRate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float bloodVolumeRatio = context.m_pBloodHitZone.GetHealthScaled();
		
		if (bloodVolumeRatio < context.m_pBloodHitZone.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_3_HEMORRHAGE))
		{
			float baseTachycardiaHR = context.m_pVitals.GetHeartRate() * 14.3 * Math.Lerp(0.5, 1, bloodVolumeRatio) / Math.Max(6, context.m_pVitals.GetMeanArterialPressure());
			
			// Apply acidosis impact on top of tachycardia
			float heartWeaknessFactor = CalculateHeartWeaknessFromAcidosis(context.m_pVitals.GetMetabolicAcidosisLevel());
			return Math.Lerp(baseTachycardiaHR, HR_MAX_ACIDOSIS_BPM, heartWeaknessFactor);
		}
		
		return super.ComputeBaseTargetHeartRate(context, timeSlice);
	}
}
