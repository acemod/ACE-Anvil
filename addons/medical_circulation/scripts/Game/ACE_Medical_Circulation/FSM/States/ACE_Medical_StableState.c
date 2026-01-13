//------------------------------------------------------------------------------------------------
class ACE_Medical_StableState : ACE_Medical_IVitalState
{
	// Metabolic acidosis constants
	static const float ACIDOSIS_STARTING_LEVEL = -1.0; // Starting acidosis level (no acidosis)
	static const float ACIDOSIS_RANGE = 2.0; // Acidosis range from -1.0 to 1.0
	static const float ACIDOSIS_MAX_LEVEL = 1.0; // Maximum acidosis level
	static const float ACIDOSIS_NO_IMPACT_THRESHOLD = 0.0; // Below this threshold, no heart impact
	static const float HR_MIN_ACIDOSIS_BPM = 50.0; // [bpm] Minimum HR at 100% metabolic acidosis in stable state
	
	// Quadratic equation coefficients: time = a*x^2 + b*x + c
	// Points: (0.2, 120), (0.4, 900), (1.0, 60)
	static const float ACIDOSIS_QUADRATIC_A = -6625.0; // x^2 coefficient
	static const float ACIDOSIS_QUADRATIC_B = 7875.0; // x coefficient
	static const float ACIDOSIS_QUADRATIC_C = -1190.0; // constant
	
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		CalculateMetabolicAcidosisRecovery(context, timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Optimized combined calculation of acidosis rate to avoid function call overhead
	//! Returns ACIDOSIS_RANGE / timeToFullEffect, with timeToFullEffect calculated from quadratic equation
	protected float CalculateAcidosisRate(float bloodRemaining)
	{
		// Clamp blood remaining to valid range [0.0, 1.0]
		float x = Math.Clamp(bloodRemaining, 0.0, 1.0);
		
		// Optimized quadratic calculation: a*x^2 + b*x + c
		// Compute x*x once and reuse
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
		
		// Extract blood volume ratio once
		float bloodVolumeRatio = context.m_pBloodHitZone.GetHealthScaled();
		float acidosisRate = CalculateAcidosisRate(bloodVolumeRatio);
		
		// Calculate delta with sign based on direction
		// sign = 1.0 if increasing, -1.0 if decreasing (using pure math: 2.0 * bool - 1.0)
		float delta = timeSlice * acidosisRate;
		float sign = 2.0 * isIncreasing - 1.0;
		float newAcidosis = Math.Clamp(currentAcidosis + (delta * sign), ACIDOSIS_STARTING_LEVEL, ACIDOSIS_MAX_LEVEL);
		
		// Only update if value has actually changed (use epsilon for floating point comparison)
		if (Math.AbsFloat(newAcidosis - currentAcidosis) > 0.0001)
		{
			context.m_pVitals.SetMetabolicAcidosisLevel(newAcidosis);
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
		float baseHR = s_pCirculationSettings.m_fDefaultHeartRateBPM;
		float heartWeaknessFactor = CalculateHeartWeaknessFromAcidosis(context.m_pVitals.GetMetabolicAcidosisLevel());
		
		// Apply acidosis impact: lerp from base HR down to minimum HR based on heart weakness
		return Math.Lerp(baseHR, HR_MIN_ACIDOSIS_BPM, heartWeaknessFactor);
	}
}
