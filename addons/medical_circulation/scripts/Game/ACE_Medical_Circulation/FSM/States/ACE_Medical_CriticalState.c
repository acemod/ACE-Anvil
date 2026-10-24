//------------------------------------------------------------------------------------------------
class ACE_Medical_CriticalState : ACE_Medical_StableState
{
	// Tachycardia constants
	static const float TACHYCARDIA_MULTIPLIER = 14.3; // Multiplier for tachycardia calculation
	static const float TACHYCARDIA_LERP_MIN = 0.5; // Minimum lerp value for tachycardia
	static const float TACHYCARDIA_LERP_MAX = 1.0; // Maximum lerp value for tachycardia
	static const float MAP_MIN_FOR_CALCULATION = 6.0; // [kPa] Minimum MAP for tachycardia calculation
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
	//! Add hypovolemic shock induced tachycardia
	override protected float ComputeBaseTargetHeartRate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float bloodVolumeRatio = context.m_pBloodHitZone.GetHealthScaled();
		
		float mapValue = Math.Max(context.m_pVitals.GetMeanArterialPressure(), MAP_MIN_FOR_CALCULATION);
		float baseTachycardiaHR = context.m_pVitals.GetHeartRate() * TACHYCARDIA_MULTIPLIER * Math.Lerp(TACHYCARDIA_LERP_MIN, TACHYCARDIA_LERP_MAX, bloodVolumeRatio) / mapValue;
		
		return Math.Lerp(baseTachycardiaHR, HR_MAX_ACIDOSIS_BPM, context.m_pVitals.GetHeartWeaknessFactor());
	}
}
