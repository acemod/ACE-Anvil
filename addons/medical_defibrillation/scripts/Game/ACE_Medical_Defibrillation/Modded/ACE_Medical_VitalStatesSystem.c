#ifdef ENABLE_DIAG
modded class ACE_Medical_VitalStatesSystem
{
    //------------------------------------------------------------------------------------------------
    //! Show vitals for target entity
    override void OnDiag(float timeSlice)
    {
        super.OnDiag(timeSlice);
        
        IEntity target;
        string targetType;
        if (!ACE_DiagTools.GetDiagTargetCharacter(target, targetType))
            return;
        
        ACE_Medical_VitalsComponent component = ACE_Medical_VitalsComponent.Cast(target.FindComponent(ACE_Medical_VitalsComponent));
        if (!component)
            return;
        
        SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
        if (!damageManager)
            return;
        
        // Get settings
        ACE_Medical_Defibrillation_Settings settings = ACE_Medical_Defibrillation_DefibComponent.GetDefibSystemSettings();
        
        DbgUI.Begin(string.Format("ACE Defibrillation - Patient: (%1)", targetType), 0, 550);
        
        // ==================== PATIENT STATUS ====================
        DbgUI.Text("=== PATIENT STATUS ===");
        DbgUI.Text(string.Format("Cardiac Rhythm:                      %1", SCR_Enum.GetEnumName(ACE_Medical_Defibrillation_ECardiacRhythm, component.GetCardiacRhythm())));
        DbgUI.Text(string.Format("Times Arrested:                      %1", ACE_Medical_Defibrillation_DiagTools.GetTimesArrested(target)));
        
        // Time in arrest
        float timeSinceArrestMs = component.GetTimeSinceArrestStart();
        float timeSinceArrestSec = timeSinceArrestMs / 1000.0;
        int minutes = Math.Floor(timeSinceArrestSec / 60);
        int seconds = Math.Floor(timeSinceArrestSec - (minutes * 60));
        DbgUI.Text(string.Format("Time in Arrest:                      %1m %2s (%3 seconds)", minutes, seconds, timeSinceArrestSec));
        
        // ==================== SHOCK TRACKING ====================
        DbgUI.Spacer(5);
        DbgUI.Text("=== SHOCK TRACKING ===");
        DbgUI.Text(string.Format("Shocks Delivered:                    %1", component.GetShocksDelivered()));
        
        float timeSinceLastShockSec = ACE_Medical_Defibrillation_DiagTools.GetTimeSinceLastShock(target);
        DbgUI.Text(string.Format("Time Since Last Shock:               %1 seconds", timeSinceLastShockSec));
        
        // ==================== SHOCK SUCCESS CALCULATION ====================
        DbgUI.Spacer(5);
        DbgUI.Text("=== SHOCK SUCCESS CALCULATION ===");
        
        float baseShockChance = ACE_Medical_Defibrillation_DecayCalculator.CalculateShockChance(component);
        float spamPenalty = ACE_Medical_Defibrillation_DecayCalculator.CalculateSpamPenalty(component);
        float finalShockChance = baseShockChance * (1.0 - spamPenalty);
        
        DbgUI.Text(string.Format("Base Shock Chance:                   %1", baseShockChance * 100));
        DbgUI.Text(string.Format("Spam Penalty:                        %1", spamPenalty * 100));
        DbgUI.Text(string.Format("Final Shock Success Chance:          %1", finalShockChance * 100));
        
        // Decay formula info
        string decayFormula = SCR_Enum.GetEnumName(ACE_Medical_Defibrillation_EDefibSettingDecayType, settings.m_eShockDecayFormula);
        DbgUI.Text(string.Format("Decay Formula:                       %1", decayFormula));
        DbgUI.Text(string.Format("Decay Rate:                          %1", settings.m_fShockSuccessDecayRate));
        
        // ==================== REVIVE BONUS CALCULATION ====================
        DbgUI.Spacer(5);
        DbgUI.Text("=== REVIVE BONUS CALCULATION ===");
        
        float reviveBonus = ACE_Medical_Defibrillation_DiagTools.GetReviveBonus(target);
        DbgUI.Text(string.Format("Revive Bonus:                        %1", reviveBonus * 100));
        
        // Bonus decay info
        string bonusDecayFormula = SCR_Enum.GetEnumName(ACE_Medical_Defibrillation_EDefibSettingDecayType, settings.m_eReviveBonusDecayFormula);
        DbgUI.Text(string.Format("Bonus Decay Formula:                 %1", bonusDecayFormula));
        DbgUI.Text(string.Format("Bonus Decay Rate:                    %1", settings.m_fReviveBonusDecayRate));
        DbgUI.Text(string.Format("Max Total Revive Bonus:              %1", settings.m_fMaxTotalReviveBonus * 100));
        
        // ==================== REVIVE CHANCE ====================
        DbgUI.Spacer(5);
        DbgUI.Text("=== REVIVE CHANCE ===");
        
        float reviveChance = ACE_Medical_Defibrillation_DiagTools.GetReviveChance(target);
        DbgUI.Text(string.Format("Total Revive Chance:                 %1", reviveChance * 100));
        DbgUI.PlotLiveClamped("Revive Chance", 500, 200, reviveChance, 0, 1, 100, 1000);
		
		DbgUI.End();
    }
}
#endif