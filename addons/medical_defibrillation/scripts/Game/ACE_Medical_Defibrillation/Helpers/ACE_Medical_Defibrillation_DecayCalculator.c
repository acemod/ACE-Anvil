//------------------------------------------------------------------------------------------------
class ACE_Medical_Defibrillation_DecayCalculator
{
    //------------------------------------------------------------------------------------------------
    static float CalculateShockChance(ACE_Medical_VitalsComponent vitals)
    {
        ACE_Medical_Defibrillation_Settings settings = ACE_Medical_Defibrillation_DefibComponent.GetDefibSystemSettings();
        
        int shocks = vitals.GetShocksDelivered();
        
        // If decay is disabled or no shocks, return max value
        if (!settings.m_bShockChanceDecay || shocks <= 0)
            return settings.m_fMaxShockSuccessChance;
        
        float result = settings.m_fMaxShockSuccessChance;
        
        switch (settings.m_eShockDecayFormula)
        {
            case ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential:
                result = settings.m_fMaxShockSuccessChance * Math.Pow(settings.m_fShockSuccessDecayRate, shocks);
                break;
                
            case ACE_Medical_Defibrillation_EDefibSettingDecayType.Linear:
                result = settings.m_fMaxShockSuccessChance - (settings.m_fShockSuccessDecayRate * shocks);
                break;
                
            case ACE_Medical_Defibrillation_EDefibSettingDecayType.Reciprocal:
                result = settings.m_fMaxShockSuccessChance / (1.0 + (settings.m_fShockSuccessDecayRate * shocks));
                break;
                
            case ACE_Medical_Defibrillation_EDefibSettingDecayType.SCurve:
                float midpoint = 3.0;
                float exponent = settings.m_fShockSuccessDecayRate * (shocks - midpoint);
                result = settings.m_fMaxShockSuccessChance / (1.0 + Math.Pow(Math.E, exponent));
                break;
        }
        
        // Clamp between min and max
        return Math.Clamp(result, settings.m_fMinShockSuccessChance, settings.m_fMaxShockSuccessChance);
    }
    
    //------------------------------------------------------------------------------------------------
    static float CalculateReviveBonus(ACE_Medical_VitalsComponent vitals)
    {
        ACE_Medical_Defibrillation_Settings settings = ACE_Medical_Defibrillation_DefibComponent.GetDefibSystemSettings();
        
        int shocks = vitals.GetShocksDelivered();
        
        // If decay is disabled or no shocks, return max value
        if (!settings.m_bReviveBonusDecay || shocks <= 0)
            return settings.m_fMaxReviveBonus;
        
        float result = settings.m_fMaxReviveBonus;
        
        switch (settings.m_eShockBonusDecayFormula)
        {
            case ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential:
                result = settings.m_fMaxReviveBonus * Math.Pow(settings.m_fShockBonusDecayRate, shocks);
                break;
                
            case ACE_Medical_Defibrillation_EDefibSettingDecayType.Linear:
                result = settings.m_fMaxReviveBonus - (settings.m_fShockBonusDecayRate * shocks);
                break;
                
            case ACE_Medical_Defibrillation_EDefibSettingDecayType.Reciprocal:
                result = settings.m_fMaxReviveBonus / (1.0 + (settings.m_fShockBonusDecayRate * shocks));
                break;
                
            case ACE_Medical_Defibrillation_EDefibSettingDecayType.SCurve:
                float midpoint = 3.0;
                float exponent = settings.m_fShockBonusDecayRate * (shocks - midpoint);
                result = settings.m_fMaxReviveBonus / (1.0 + Math.Pow(Math.E, exponent));
                break;
        }
        
        // Clamp between min and max
        return Math.Clamp(result, settings.m_fMinReviveBonus, settings.m_fMaxReviveBonus);
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