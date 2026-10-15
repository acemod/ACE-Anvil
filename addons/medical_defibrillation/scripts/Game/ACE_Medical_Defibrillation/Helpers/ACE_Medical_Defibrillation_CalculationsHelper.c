//------------------------------------------------------------------------------------------------
class ACE_Medical_Defibrillation_CalculationsHelper
{	
	//------------------------------------------------------------------------------------------------
	static float CalculateTotalShockChance(ACE_Medical_VitalsComponent vitals, ACE_Medical_Defibrillation_Settings settings)
	{
	    int shocks = vitals.GetShocksDelivered();
	    float timeSinceLastShock = vitals.GetTimeSinceLastShock();
	    
	    float spamPenalty = ACE_Medical_Defibrillation_CalculationsHelper.CalculateSpamPenalty(vitals, settings);
	    float shockChance = ACE_Medical_Defibrillation_CalculationsHelper.CalculateShockChance(vitals, settings);
	    
	    float finalChance = shockChance * (1.0 - spamPenalty);
	    
	    return Math.Clamp(finalChance, 0.0, settings.m_fBaseShockSuccessChance);
	}
	
	//------------------------------------------------------------------------------------------------
	static float CalculateShockChance(ACE_Medical_VitalsComponent vitals, ACE_Medical_Defibrillation_Settings settings)
	{
	    int shocks = vitals.GetShocksDelivered();
	    
	    // Handle TimeBased separately - uses time since arrest started
	    if (settings.m_eShockDecayFormula == ACE_Medical_Defibrillation_EDefibSettingDecayType.TimeBased)
	    {
	        return CalculateTimeBasedShockChance(vitals, settings);
	    }
		
	    // If decay is disabled or no shocks, return base value
	    if (!settings.m_bShockChanceDecay || shocks <= 0)
	        return settings.m_fBaseShockSuccessChance;
	    
	    float result = settings.m_fBaseShockSuccessChance;
	    
	    switch (settings.m_eShockDecayFormula)
	    {
	        case ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential:
	            result = settings.m_fBaseShockSuccessChance * Math.Pow(settings.m_fShockSuccessDecayRate, shocks);
	            break;
	            
	        case ACE_Medical_Defibrillation_EDefibSettingDecayType.Linear:
	            result = settings.m_fBaseShockSuccessChance - (settings.m_fShockSuccessDecayRate * shocks);
	            break;
	            
	        case ACE_Medical_Defibrillation_EDefibSettingDecayType.Reciprocal:
	            result = settings.m_fBaseShockSuccessChance / (1.0 + (settings.m_fShockSuccessDecayRate * shocks));
	            break;
	            
	        case ACE_Medical_Defibrillation_EDefibSettingDecayType.SCurve:
	            float midpoint = 3.0;
	            float exponent = settings.m_fShockSuccessDecayRate * (shocks - midpoint);
	            result = settings.m_fBaseShockSuccessChance / (1.0 + Math.Pow(Math.E, exponent));
	            break;
	    }
	    
	    return Math.Clamp(result, settings.m_fMinShockSuccessChance, settings.m_fBaseShockSuccessChance);
	}
	
	//------------------------------------------------------------------------------------------------
	static float CalculateReviveBonus(ACE_Medical_VitalsComponent vitals, ACE_Medical_Defibrillation_Settings settings)
	{
		int shocks = vitals.GetShocksDelivered();
	    
	    // Handle TimeBased separately - uses time since arrest started
	    if (settings.m_eReviveBonusDecayFormula == ACE_Medical_Defibrillation_EDefibSettingDecayType.TimeBased)
	    {
	        return CalculateTimeBasedReviveBonus(vitals, settings);
	    }
		
	    // If decay is disabled or no shocks, return base value
	    if (!settings.m_bReviveBonusDecay || shocks <= 0)
	        return settings.m_fBaseReviveBonus;
	    
	    float totalBonus = 0;
	    
	    for (int i = 1; i <= shocks; i++)
	    {
	        float shockBonus = settings.m_fBaseReviveBonus;
	        
	        switch (settings.m_eReviveBonusDecayFormula)
	        {
	            case ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential:
	                shockBonus = settings.m_fBaseReviveBonus * Math.Pow(settings.m_fReviveBonusDecayRate, i - 1);
	                break;
	                
	            case ACE_Medical_Defibrillation_EDefibSettingDecayType.Linear:
	                shockBonus = settings.m_fBaseReviveBonus - (settings.m_fReviveBonusDecayRate * (i - 1));
	                break;
	                
	            case ACE_Medical_Defibrillation_EDefibSettingDecayType.Reciprocal:
	                shockBonus = settings.m_fBaseReviveBonus / (1.0 + (settings.m_fReviveBonusDecayRate * (i - 1)));
	                break;
	                
	            case ACE_Medical_Defibrillation_EDefibSettingDecayType.SCurve:
	                float midpoint = 3.0;
	                float exponent = settings.m_fReviveBonusDecayRate * ((i - 1) - midpoint);
	                shockBonus = settings.m_fBaseReviveBonus / (1.0 + Math.Pow(Math.E, exponent));
	                break;
	        }
	        
	        shockBonus = Math.Max(shockBonus, 0);
	        totalBonus += shockBonus;
	        
	        if (totalBonus >= settings.m_fMaxTotalReviveBonus)
	            break;
	    }
	    
	    return Math.Min(totalBonus, settings.m_fMaxTotalReviveBonus);
	}
	
	//------------------------------------------------------------------------------------------------
	static float CalculateTimeBasedShockChance(ACE_Medical_VitalsComponent vitals, ACE_Medical_Defibrillation_Settings settings)
	{
		if (!settings.m_bShockChanceDecay)
			return settings.m_fBaseShockSuccessChance;
		
	    int timesArrest = vitals.GetTimesArrested();
	    if (timesArrest <= 0)
	        return settings.m_fBaseShockSuccessChance;
	    
	    float timeSinceArrest = vitals.GetTimeSinceArrestStart();
	    float timeSeconds = timeSinceArrest / 1000.0;
	    
	    float decayTime = settings.m_fShockSuccessDecayRate;
	    
	    if (decayTime <= 0 || timeSeconds <= 0)
	        return settings.m_fBaseShockSuccessChance;
	    
	    if (timeSeconds >= decayTime)
	        return settings.m_fMinShockSuccessChance;
	    
	    float ratio = 1.0 - (timeSeconds / decayTime);
	    float result = settings.m_fBaseShockSuccessChance * ratio;
	    
	    return Math.Clamp(result, settings.m_fMinShockSuccessChance, settings.m_fBaseShockSuccessChance);
	}
	
	//------------------------------------------------------------------------------------------------
	static float CalculateTimeBasedReviveBonus(ACE_Medical_VitalsComponent vitals, ACE_Medical_Defibrillation_Settings settings)
	{
		if (!settings.m_bReviveBonusDecay)
			return settings.m_fBaseReviveBonus;
		
	    int timesArrest = vitals.GetTimesArrested();
	    if (timesArrest <= 0)
	        return settings.m_fMaxTotalReviveBonus;
	    
	    float timeSinceArrest = vitals.GetTimeSinceArrestStart();
	    float timeSeconds = timeSinceArrest / 1000.0;
	    
	    float decayTime = settings.m_fReviveBonusDecayRate;
	    
	    if (decayTime <= 0 || timeSeconds <= 0)
	        return settings.m_fMaxTotalReviveBonus;
	    
	    if (timeSeconds >= decayTime)
	        return 0;
	    
	    float ratio = 1.0 - (timeSeconds / decayTime);
	    float result = settings.m_fMaxTotalReviveBonus * ratio;
	    
	    return Math.Clamp(result, 0, settings.m_fMaxTotalReviveBonus);
	}
    
    //------------------------------------------------------------------------------------------------
    static float CalculateSpamPenalty(ACE_Medical_VitalsComponent vitals, ACE_Medical_Defibrillation_Settings settings)
    {
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