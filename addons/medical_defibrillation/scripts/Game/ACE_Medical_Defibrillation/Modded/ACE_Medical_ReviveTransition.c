//------------------------------------------------------------------------------------------------
modded class ACE_Medical_ReviveTransition
{
    protected static ACE_Medical_Defibrillation_Settings s_pDefibSettings;
    
    //------------------------------------------------------------------------------------------------
    void ACE_Medical_ReviveTransition(ACE_FSM_EStateID fromStateIDs, ACE_FSM_EStateID toStateID)
    {
        if (!s_pDefibSettings)
            s_pDefibSettings = ACE_SettingsHelperT<ACE_Medical_Defibrillation_Settings>.GetModSettings();
    }
    
    //------------------------------------------------------------------------------------------------
    float ComputeReviveBonus(ACE_Medical_CharacterContext context)
    {
        if (!s_pDefibSettings)
            return 0;
        
        int shocks = context.m_pVitals.GetShocksDelivered();
        
        if (shocks == 0)
            return 0;
        
        float reviveBonus = ACE_Medical_Defibrillation_DecayCalculator.CalculateReviveBonus(context.m_pVitals);
        
        return reviveBonus;
    }
    
    //------------------------------------------------------------------------------------------------
    override float ComputeReviveChance(ACE_Medical_CharacterContext context)
    {
        float result = super.ComputeReviveChance(context);
        
        if (!s_pDefibSettings || !s_pDefibSettings.m_bEnabled)
            return result;
        
        bool isDefibMandatory = s_pDefibSettings.m_bDefibIsMandatory;
        int shocks = context.m_pVitals.GetShocksDelivered();
        
        if (isDefibMandatory && shocks == 0)
            return 0;
        
        float reviveBonus = ComputeReviveBonus(context);
        result += reviveBonus;
        result = Math.Min(result, 1.0);
        
        return result;
    }
}