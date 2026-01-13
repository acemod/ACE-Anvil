//------------------------------------------------------------------------------------------------
class ACE_Medical_StableState : ACE_Medical_IVitalState
{
	// Metabolic acidosis constants
	static const float ACIDOSIS_STARTING_LEVEL = -1.0; // Starting acidosis level (no acidosis)
	static const float ACIDOSIS_RANGE = 2.0; // Acidosis range from -1.0 to 1.0
	static const float ACIDOSIS_MAX_LEVEL = 1.0; // Maximum acidosis level
	static const float ACIDOSIS_NO_IMPACT_THRESHOLD = 0.0; // Below this threshold, no heart impact
	
	// Single quadratic equation coefficients for time to full effect
	// Points: (0.0, 300), (0.5, 900), (1.0, 300) with maximum at 50%
	// Quadratic: y = ax^2 + bx + c
	static const float ACIDOSIS_QUADRATIC_A = -2400.0; // x^2 coefficient
	static const float ACIDOSIS_QUADRATIC_B = 2400.0; // x coefficient
	static const float ACIDOSIS_QUADRATIC_C = 300.0; // constant
	static const float ACIDOSIS_NO_CHANGE_THRESHOLD = 0.6; // Blood volume threshold for no-change (60%)
	
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		CalculateMetabolicAcidosisRecovery(context, timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Optimized combined calculation of acidosis rate to avoid function call overhead
	//! Returns ACIDOSIS_RANGE / timeToFullEffect, with timeToFullEffect calculated from single quadratic
	//! Quadratic: y = ax^2 + bx + c with maximum at 50% blood volume
	//! Points: (0.0, 300s), (0.5, 900s), (1.0, 300s)
	protected float CalculateAcidosisRate(float bloodRemaining)
	{
		// Clamp blood remaining to valid range [0.0, 1.0]
		float x = Math.Clamp(bloodRemaining, 0.0, 1.0);
		
		// Calculate time to full effect using single quadratic
		float xSquared = x * x;
		float timeToFull = ACIDOSIS_QUADRATIC_A * xSquared + ACIDOSIS_QUADRATIC_B * x + ACIDOSIS_QUADRATIC_C;
		
		// Ensure minimum time of 1.0 to prevent division by zero or negative rates
		timeToFull = Math.Max(timeToFull, 1.0);
		
		// Return rate directly: range / time
		return ACIDOSIS_RANGE / timeToFull;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Calculate heart weakness factor from acidosis level
	//! Optimized: uses early return and combined clamp
	protected float CalculateHeartWeaknessFromAcidosis(float acidosisLevel)
	{
		// Early return if below impact threshold (no weakness)
		if (acidosisLevel <= ACIDOSIS_NO_IMPACT_THRESHOLD)
			return 0.0;
		// Clamp acidosis to [0, 1] for heart weakness calculation
		return Math.Min(acidosisLevel, 1.0);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Updates metabolic acidosis level based on blood volume and time
	//! @param isIncreasing True for critical states (acidosis builds up), false for stable/unstable states (acidosis recovers)
	protected void UpdateMetabolicAcidosis(ACE_Medical_CharacterContext context, float timeSlice, bool isIncreasing)
	{
		float currentAcidosis = context.m_pVitals.GetMetabolicAcidosisLevel();
		
		// Exit if: (increasing and at max) OR (decreasing and at min)
		if ((isIncreasing && currentAcidosis >= ACIDOSIS_MAX_LEVEL) || (!isIncreasing && currentAcidosis <= ACIDOSIS_STARTING_LEVEL))
			return;
		
		// Cache current weakness factor to compare later
		float currentWeakness = context.m_pVitals.GetHeartWeaknessFactor();
		
		// Extract blood volume ratio once
		float bloodVolumeRatio = context.m_pBloodHitZone.GetHealthScaled();
		float acidosisRate = CalculateAcidosisRate(bloodVolumeRatio);
		
		// Calculate delta with sign based on direction
		// sign = 1.0 if increasing, -1.0 if decreasing (using pure math: 2.0 * bool - 1.0)
		float delta = timeSlice * acidosisRate;
		float sign = 2.0 * isIncreasing - 1.0;
		float newAcidosis = Math.Clamp(currentAcidosis + (delta * sign), ACIDOSIS_STARTING_LEVEL, ACIDOSIS_MAX_LEVEL);
		
		// Only update if value has actually changed
		if (newAcidosis != currentAcidosis)
		{
			context.m_pVitals.SetMetabolicAcidosisLevel(newAcidosis);
			
			// Recalculate weakness factor and update only if changed
			float newWeakness = CalculateHeartWeaknessFromAcidosis(newAcidosis);
			if (newWeakness != currentWeakness)
				context.m_pVitals.SetHeartWeaknessFactor(newWeakness);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CalculateMetabolicAcidosisRecovery(ACE_Medical_CharacterContext context, float timeSlice)
	{
		UpdateMetabolicAcidosis(context, timeSlice, false);
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
