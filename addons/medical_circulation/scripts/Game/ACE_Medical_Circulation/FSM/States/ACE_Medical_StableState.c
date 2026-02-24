//------------------------------------------------------------------------------------------------
class ACE_Medical_StableState : ACE_Medical_IVitalState
{
	// Blood pH constants (physiological scale)
	static const float PH_NORMAL = 7.4; // Normal arterial pH
	static const float PH_IMPACT_THRESHOLD = 7.35; // Below this, heart weakness applies
	static const float PH_MIN = 6.8; // Maximum acidosis (severe)
	static const float PH_RANGE = 0.6; // PH_NORMAL - PH_MIN
	
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
	//! pH change rate (pH units per second) from blood volume; same quadratic time-to-full as before
	//! Returns PH_RANGE / timeToFullEffect
	protected float CalculateAcidosisRate(float bloodRemaining)
	{
		float x = Math.Clamp(bloodRemaining, 0.0, 1.0);
		float xSquared = x * x;
		float timeToFull = ACIDOSIS_QUADRATIC_A * xSquared + ACIDOSIS_QUADRATIC_B * x + ACIDOSIS_QUADRATIC_C;
		timeToFull = Math.Max(timeToFull, 1.0);
		return PH_RANGE / timeToFull;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Heart weakness factor from blood pH: 0 when pH >= 7.35, linear 0..1 from 7.35 down to 6.8
	protected float CalculateHeartWeaknessFromPH(float pH)
	{
		if (pH >= PH_IMPACT_THRESHOLD)
			return 0.0;
		float weakness = (PH_IMPACT_THRESHOLD - pH) / (PH_IMPACT_THRESHOLD - PH_MIN);
		return Math.Clamp(weakness, 0.0, 1.0);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Updates blood pH based on blood volume and time (worsening = pH down, recovery = pH up)
	//! @param isIncreasing True for critical/cardiac arrest (acidosis worsens, pH decreases), false for recovery
	protected void UpdateMetabolicAcidosis(ACE_Medical_CharacterContext context, float timeSlice, bool isIncreasing)
	{
		float currentPH = context.m_pVitals.GetBloodPH();
		
		if ((isIncreasing && currentPH <= PH_MIN) || (!isIncreasing && currentPH >= PH_NORMAL))
			return;
		
		float currentWeakness = context.m_pVitals.GetHeartWeaknessFactor();
		float bloodVolumeRatio = context.m_pBloodHitZone.GetHealthScaled();
		float pHRate = CalculateAcidosisRate(bloodVolumeRatio);
		float delta = timeSlice * pHRate;
		// Worsening: pH decreases; recovery: pH increases
		float newPH = isIncreasing
			? Math.Clamp(currentPH - delta, PH_MIN, PH_NORMAL)
			: Math.Clamp(currentPH + delta, PH_MIN, PH_NORMAL);
		
		if (newPH != currentPH)
		{
			context.m_pVitals.SetBloodPH(newPH);
			float newWeakness = CalculateHeartWeaknessFromPH(newPH);
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
