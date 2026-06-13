//------------------------------------------------------------------------------------------------
class ACE_Medical_Defibrillation_DecayCalculator
{
    //------------------------------------------------------------------------------------------------
    static float Calculate(
        int shocks,
        float maxValue,
        float minValue,
        bool bEnableDecay,
        ACE_Medical_Defibrillation_EDefibSettingDecayType decayType,
        float decayRate
    )
    {
        // If decay is disabled or no shocks, return max value
        if (!bEnableDecay || shocks <= 1)
            return maxValue;
        
        float result = maxValue;
        int shockIndex = shocks - 1; // First shock = index 0
        
        switch (decayType)
        {
            case ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential:
                // value = max * (rate ^ shocks)
                result = maxValue * Math.Pow(decayRate, shockIndex);
                break;
                
            case ACE_Medical_Defibrillation_EDefibSettingDecayType.Linear:
                // value = max - (rate * shocks)
                result = maxValue - (decayRate * shockIndex);
                break;
                
            case ACE_Medical_Defibrillation_EDefibSettingDecayType.Reciprocal:
                // value = max / (1 + rate * shocks)
                result = maxValue / (1.0 + (decayRate * shockIndex));
                break;
                
			case ACE_Medical_Defibrillation_EDefibSettingDecayType.SCurve:
			    // value = max / (1 + e^(rate * (shocks - midpoint)))
			    float midpoint = 3.0;
			    float exponent = decayRate * (shockIndex - midpoint);
			    result = maxValue / (1.0 + Math.Pow(Math.E, exponent));
			    break;
        }
        
        // Clamp between min and max
        return Math.Clamp(result, minValue, maxValue);
    }
    
    //------------------------------------------------------------------------------------------------
	static float CalculateSpamPenalty(ACE_Medical_VitalsComponent vitals)
	{
		ACE_Medical_Defibrillation_Settings settings = ACE_Medical_Defibrillation_DefibComponent.GetDefibSystemSettings();
		
		bool punishSpam = settings.m_bPunishSpamShocks;
		float timeoutSeconds = settings.m_fPunishSpamShocksTimeout;
		float timeSinceLastShock = vitals.GetTimeSinceLastShock();
		int timesArrested = vitals.GetTimesArrested();
		
	    if (!punishSpam || timeoutSeconds <= 0 || timesArrested <= 0)
	        return 0.0; // No penalty
	    
	    float timeoutMs = timeoutSeconds * 1000;
	    
	    if (timeSinceLastShock >= timeoutMs)
	        return 0.0; // No penalty (waited long enough)
	    
	    // Linear penalty: 0 = no penalty, 1 = full penalty
	    float penalty = 1.0 - (timeSinceLastShock / timeoutMs);
	    return Math.Clamp(penalty, 0.0, 1.0);
	}
}