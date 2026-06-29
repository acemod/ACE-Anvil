//------------------------------------------------------------------------------------------------
//! Shared metabolic acidosis constants and simulation helpers.
class ACE_Medical_Acidosis
{
	// Blood pH constants (physiological scale)
	protected static const float PH_NORMAL = 7.4; // Normal arterial pH
	protected static const float PH_IMPACT_THRESHOLD = 7.35; // Below this, heart weakness applies
	protected static const float PH_MIN = 6.8; // Maximum acidosis (severe)
	protected static const float PH_RANGE = 0.6; // PH_NORMAL - PH_MIN
	
	// Single quadratic equation coefficients for time to full effect
	// Points: (0.0, 300), (0.5, 900), (1.0, 300) with maximum at 50%
	// Quadratic: y = ax^2 + bx + c
	protected static const float ACIDOSIS_QUADRATIC_A = -2400.0; // x^2 coefficient
	protected static const float ACIDOSIS_QUADRATIC_B = 2400.0; // x coefficient
	protected static const float ACIDOSIS_QUADRATIC_C = 300.0; // constant
	protected static const float ACIDOSIS_NO_CHANGE_THRESHOLD = 0.6; // Blood volume threshold for no-change (60%)
	
	//------------------------------------------------------------------------------------------------
	//! pH change rate (pH units per second) from blood volume; same quadratic time-to-full as before
	//! Returns PH_RANGE / timeToFullEffect
	static float CalculateAcidosisRate(float bloodRemaining)
	{
		float x = Math.Clamp(bloodRemaining, 0.0, 1.0);
		float xSquared = x * x;
		float timeToFull = ACIDOSIS_QUADRATIC_A * xSquared + ACIDOSIS_QUADRATIC_B * x + ACIDOSIS_QUADRATIC_C;
		timeToFull = Math.Max(timeToFull, 1.0);
		return PH_RANGE / timeToFull;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Heart weakness factor from blood pH: 0 when pH >= 7.35, linear 0..1 from 7.35 down to 6.8
	static float CalculateHeartWeaknessFromPH(float pH)
	{
		if (pH >= PH_IMPACT_THRESHOLD)
			return 0.0;
		float weakness = (PH_IMPACT_THRESHOLD - pH) / (PH_IMPACT_THRESHOLD - PH_MIN);
		return Math.Clamp(weakness, 0.0, 1.0);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Updates blood pH based on blood volume and time (worsening = pH down, recovery = pH up)
	//! @param isIncreasing True for critical/cardiac arrest (acidosis worsens, pH decreases), false for recovery
	static void UpdateMetabolicAcidosis(ACE_Medical_CharacterContext context, float timeSlice, bool isIncreasing)
	{
		float currentPH = context.m_pVitals.GetBloodPH();
		
		if ((isIncreasing && currentPH <= PH_MIN) || (!isIncreasing && currentPH >= PH_NORMAL))
			return;
		
		float currentWeakness = context.m_pVitals.GetHeartWeaknessFactor();
		float bloodVolumeRatio = context.m_pBloodHitZone.GetHealthScaled();
		float pHRate = CalculateAcidosisRate(bloodVolumeRatio);
		float delta = timeSlice * pHRate;
		// Worsening: pH decreases; recovery: pH increases
		float computedPH = currentPH + delta;
		if (isIncreasing)
			computedPH = currentPH - delta;
		float newPH = Math.Clamp(computedPH, PH_MIN, PH_NORMAL);
		
		if (newPH != currentPH)
		{
			context.m_pVitals.SetBloodPH(newPH);
			float newWeakness = CalculateHeartWeaknessFromPH(newPH);
			if (newWeakness != currentWeakness)
				context.m_pVitals.SetHeartWeaknessFactor(newWeakness);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	static void CalculateMetabolicAcidosisRecovery(ACE_Medical_CharacterContext context, float timeSlice)
	{
		UpdateMetabolicAcidosis(context, timeSlice, false);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Helper for states where acidosis changes only when blood volume is above or below threshold
	static void UpdateMetabolicAcidosisWithThreshold(ACE_Medical_CharacterContext context, float timeSlice, bool isIncreasing, bool requireBloodVolumeAboveThreshold)
	{
		float bloodVolumeRatio = context.m_pBloodHitZone.GetHealthScaled();
		bool isBloodVolumeAboveThreshold = bloodVolumeRatio > ACIDOSIS_NO_CHANGE_THRESHOLD;
		if (isBloodVolumeAboveThreshold != requireBloodVolumeAboveThreshold)
			return;
		
		UpdateMetabolicAcidosis(context, timeSlice, isIncreasing);
	}
}
