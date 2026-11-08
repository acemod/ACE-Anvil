/*
//=============================================================================
// ACE Medical Defibrillation System - Mission Header
//=============================================================================
// 
// IMPORTANT: Due to engine limitation (feedback.bistudio.com/T184217),
// ALL settings MUST be explicitly defined. Omitted values will default to 0/false.
//
// Copy this entire block into your missionHeader.json:
//
// "m_ACE_Settings": {
//     "m_ACE_Medical_Defibrillation": {
//         "m_bEnabled": true,
//         "m_bDefibIsMandatory": false,
//         "m_eDefibSettingPreset": 1,
//         "m_fAED_AnalysisDuration": 3.0,
//         "m_fAED_CPRCooldownDuration": 120.0,
//         "m_bAED_PlayCPRPacingBeats": true,
//         "m_bPunishSpamShocks": true,
//         "m_fPunishSpamShocksTimeout": 108.0,
//         "m_bShockChanceDecay": true,
//         "m_fShockSuccessDecayRate": 0.92,
//         "m_eShockDecayFormula": 0,
//         "m_fBaseShockSuccessChance": 0.9,
//         "m_fMinShockSuccessChance": 0.0,
//         "m_bReviveBonusDecay": true,
//         "m_fReviveBonusDecayRate": 0.85,
//         "m_eReviveBonusDecayFormula": 0,
//         "m_fBaseReviveBonus": 0.25,
//         "m_fMaxTotalReviveBonus": 0.9
//     }
// }
//
// Preset Values (m_eDefibSettingPreset):
// 0 = Custom    - Use custom settings (other settings ignore everything below the preset setting)
// 1 = Realistic - Balanced, forgiving for CPR
// 2 = Casual    - Very forgiving
// 3 = Hardcore  - Punishing but still accounts for CPR
// 4 = Arcade    - Power fantasy
// 5 = TimeBased - 8-minute decay window
//
// Decay Formula Values (m_eShockDecayFormula / m_eReviveBonusDecayFormula):
// 0 = Exponential  - Value × rate^shocks (smooth curve)
// 1 = Linear       - Value - (rate × shocks) (straight line)
// 2 = Reciprocal   - Value / (1 + rate × shocks) (fast initial drop)
// 3 = SCurve       - Logistic curve (holds then drops sharply)
// 4 = TimeBased    - Based on time since arrest started
//
//=============================================================================
*/

enum ACE_Medical_Defibrillation_EDefibSettingPreset
{
    Custom,
    Realistic,
    Casual,
    Hardcore,
    Arcade,
    TimeBased
}

enum ACE_Medical_Defibrillation_EDefibSettingDecayType
{
    Exponential,
    Linear,
    Reciprocal,
    SCurve,
    TimeBased
}

//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Medical_Defibrillation_Settings : ACE_ModSettings
{
    // Core Settings
    [Attribute(defvalue: "true", desc: "Enables the defibrillation system.", category: "Core Settings")]
    bool m_bEnabled;
    
    [Attribute(defvalue: "false", desc: "Defibrillation is mandatory for revive.", category: "Core Settings")]
    bool m_bDefibIsMandatory;
    
    [Attribute(defvalue: "3", params: "3 inf", desc: "Time (s) between AED analysis events where players should perform CPR.", category: "AED Settings")]
    float m_fAED_AnalysisDuration;
    
    [Attribute(defvalue: "1", uiwidget: UIWidgets.ComboBox, desc: "Uses a preset and ignores custom settings below.", enums: ParamEnumArray.FromEnum(ACE_Medical_Defibrillation_EDefibSettingPreset), category: "Core Settings")]
    ACE_Medical_Defibrillation_EDefibSettingPreset m_eDefibSettingPreset;
    
    [Attribute(defvalue: "120", params: "0 inf", desc: "Time (s) between AED analysis events where players should perform CPR.", category: "AED Settings")]
    float m_fAED_CPRCooldownDuration;
    
    [Attribute(defvalue: "true", desc: "Plays pacing beats when players should be performing CPR.", category: "AED Settings")]
    bool m_bAED_PlayCPRPacingBeats;

    // Shock Success
    [Attribute(defvalue: "true", desc: "Fails to deliver a successful shock if shocks are too close together.", category: "Shock Settings")]
    bool m_bPunishSpamShocks;
    
    [Attribute(defvalue: "108", params: "0 inf", desc: "Seconds since last shock to allow for successful shock with no time interval punishment.", category: "Shock Settings")]
    float m_fPunishSpamShocksTimeout;
    
    [Attribute(defvalue: "true", desc: "Shock chance decays with subsequent shocks.", category: "Shock Settings")]
    bool m_bShockChanceDecay;
    
    [Attribute(defvalue: "0.92", desc: "Shock success decay rate. (Seconds till 0% chance for TimeBased formulas)", category: "Shock Settings")]
    float m_fShockSuccessDecayRate;
    
    [Attribute(defvalue: "0", uiwidget: UIWidgets.ComboBox, desc: "Decay formula to apply to the success rate of subsequent shocks.", enums: ParamEnumArray.FromEnum(ACE_Medical_Defibrillation_EDefibSettingDecayType), category: "Shock Settings")]
    ACE_Medical_Defibrillation_EDefibSettingDecayType m_eShockDecayFormula;

    [Attribute(defvalue: "0.9", desc: "Base shock success chance for first shock.", params: "0 1", category: "Shock Settings")]
    float m_fBaseShockSuccessChance;
    
    [Attribute(defvalue: "0", desc: "Minimum shock success chance.", params: "0 1", category: "Shock Settings")]
    float m_fMinShockSuccessChance;

    // Revive Bonus
    [Attribute(defvalue: "true", desc: "Revive bonus decays with subsequent shocks.", category: "Bonus Settings")]
    bool m_bReviveBonusDecay;
    
    [Attribute(defvalue: "0.85", desc: "Revive bonus decay rate. (Seconds till 0% chance for TimeBased formulas)", category: "Bonus Settings")]
    float m_fReviveBonusDecayRate;
    
    [Attribute(defvalue: "0", uiwidget: UIWidgets.ComboBox, desc: "Decay formula to apply to the bonus from subsequent shocks.", enums: ParamEnumArray.FromEnum(ACE_Medical_Defibrillation_EDefibSettingDecayType), category: "Bonus Settings")]
    ACE_Medical_Defibrillation_EDefibSettingDecayType m_eReviveBonusDecayFormula;

    [Attribute(defvalue: "0.25", desc: "Base revive bonus for first shock.", params: "0 1", category: "Bonus Settings")]
    float m_fBaseReviveBonus;
    
    [Attribute(defvalue: "0.9", desc: "Maximum total revive bonus cap (never exceed this value).", params: "0 1", category: "Bonus Settings")]
    float m_fMaxTotalReviveBonus;
	
	const float m_fDefibChargeTime = 5.5;
	const float m_fDefibrillatorConnectionRange = 3;
    
    // Realistic Preset (1) - Balanced, forgiving for CPR
    const float AED_ANALYSIS_DURATION_REALISTIC = 3;
    const float AED_CPR_COOLDOWN_DURATION_REALISTIC = 120;
    const bool AED_PLAY_CPR_PACING_BEATS_REALISTIC = true;
    const bool PUNISH_SPAM_SHOCKS_REALISTIC = true;
    const float PUNISH_SPAM_SHOCKS_COOLDOWN_REALISTIC = 108.0;
    const bool SHOCK_CHANCE_DECAY_REALISTIC = true;
    const float SHOCK_SUCCESS_DECAY_RATE_REALISTIC = 0.92;
    const ACE_Medical_Defibrillation_EDefibSettingDecayType SHOCK_DECAY_FORMULA_REALISTIC = ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential;
    const float BASE_SHOCK_SUCCESS_CHANCE_REALISTIC = 0.90;
    const float MIN_SHOCK_SUCCESS_CHANCE_REALISTIC = 0.0;
    const bool REVIVE_BONUS_DECAY_REALISTIC = true;
    const float REVIVE_BONUS_DECAY_RATE_REALISTIC = 0.85;
    const ACE_Medical_Defibrillation_EDefibSettingDecayType REVIVE_BONUS_DECAY_FORMULA_REALISTIC = ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential;
    const float BASE_REVIVE_BONUS_REALISTIC = 0.25;
    const float MAX_TOTAL_REVIVE_BONUS_REALISTIC = 0.9;
    
    // Casual Preset (2) - Very forgiving
    const float AED_ANALYSIS_DURATION_CASUAL = 3;
    const float AED_CPR_COOLDOWN_DURATION_CASUAL = 120;
    const bool AED_PLAY_CPR_PACING_BEATS_CASUAL = true;
    const bool PUNISH_SPAM_SHOCKS_CASUAL = true;
    const float PUNISH_SPAM_SHOCKS_COOLDOWN_CASUAL = 60.0;
    const bool SHOCK_CHANCE_DECAY_CASUAL = true;
    const float SHOCK_SUCCESS_DECAY_RATE_CASUAL = 0.96;
    const ACE_Medical_Defibrillation_EDefibSettingDecayType SHOCK_DECAY_FORMULA_CASUAL = ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential;
    const float BASE_SHOCK_SUCCESS_CHANCE_CASUAL = 0.95;
    const float MIN_SHOCK_SUCCESS_CHANCE_CASUAL = 0.0;
    const bool REVIVE_BONUS_DECAY_CASUAL = true;
    const float REVIVE_BONUS_DECAY_RATE_CASUAL = 0.92;
    const ACE_Medical_Defibrillation_EDefibSettingDecayType REVIVE_BONUS_DECAY_FORMULA_CASUAL = ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential;
    const float BASE_REVIVE_BONUS_CASUAL = 0.35;
    const float MAX_TOTAL_REVIVE_BONUS_CASUAL = 1.0;
    
    // Hardcore Preset (3) - Punishing but still accounts for CPR
    const float AED_ANALYSIS_DURATION_HARDCORE = 5;
    const float AED_CPR_COOLDOWN_DURATION_HARDCORE = 180;
    const bool AED_PLAY_CPR_PACING_BEATS_HARDCORE = true;
    const bool PUNISH_SPAM_SHOCKS_HARDCORE = true;
    const float PUNISH_SPAM_SHOCKS_COOLDOWN_HARDCORE = 180.0;
    const bool SHOCK_CHANCE_DECAY_HARDCORE = true;
    const float SHOCK_SUCCESS_DECAY_RATE_HARDCORE = 0.85;
    const ACE_Medical_Defibrillation_EDefibSettingDecayType SHOCK_DECAY_FORMULA_HARDCORE = ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential;
    const float BASE_SHOCK_SUCCESS_CHANCE_HARDCORE = 0.85;
    const float MIN_SHOCK_SUCCESS_CHANCE_HARDCORE = 0.0;
    const bool REVIVE_BONUS_DECAY_HARDCORE = true;
    const float REVIVE_BONUS_DECAY_RATE_HARDCORE = 0.75;
    const ACE_Medical_Defibrillation_EDefibSettingDecayType REVIVE_BONUS_DECAY_FORMULA_HARDCORE = ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential;
    const float BASE_REVIVE_BONUS_HARDCORE = 0.20;
    const float MAX_TOTAL_REVIVE_BONUS_HARDCORE = 0.8;
    
    // Arcade Preset (4) - Power fantasy
    const float AED_ANALYSIS_DURATION_ARCADE = 2;
    const float AED_CPR_COOLDOWN_DURATION_ARCADE = 60;
    const bool AED_PLAY_CPR_PACING_BEATS_ARCADE = false;
    const bool PUNISH_SPAM_SHOCKS_ARCADE = false;
    const float PUNISH_SPAM_SHOCKS_COOLDOWN_ARCADE = 0;
    const bool SHOCK_CHANCE_DECAY_ARCADE = false;
    const float SHOCK_SUCCESS_DECAY_RATE_ARCADE = 1.0;
    const ACE_Medical_Defibrillation_EDefibSettingDecayType SHOCK_DECAY_FORMULA_ARCADE = ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential;
    const float BASE_SHOCK_SUCCESS_CHANCE_ARCADE = 1.0;
    const float MIN_SHOCK_SUCCESS_CHANCE_ARCADE = 1.0;
    const bool REVIVE_BONUS_DECAY_ARCADE = true;
    const float REVIVE_BONUS_DECAY_RATE_ARCADE = 1.20;
    const ACE_Medical_Defibrillation_EDefibSettingDecayType REVIVE_BONUS_DECAY_FORMULA_ARCADE = ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential;
    const float BASE_REVIVE_BONUS_ARCADE = 0.30;
    const float MAX_TOTAL_REVIVE_BONUS_ARCADE = 1.0;
    
    // TimeBased Preset (5) - 8 minutes for both shock success and revive bonus
    const float AED_ANALYSIS_DURATION_TIMEBASED = 3;
    const float AED_CPR_COOLDOWN_DURATION_TIMEBASED = 120;
    const bool AED_PLAY_CPR_PACING_BEATS_TIMEBASED = true;
    const bool PUNISH_SPAM_SHOCKS_TIMEBASED = true;
    const float PUNISH_SPAM_SHOCKS_COOLDOWN_TIMEBASED = 30.0;
    const bool SHOCK_CHANCE_DECAY_TIMEBASED = false;
    const float SHOCK_SUCCESS_DECAY_RATE_TIMEBASED = 480.0;
    const ACE_Medical_Defibrillation_EDefibSettingDecayType SHOCK_DECAY_FORMULA_TIMEBASED = ACE_Medical_Defibrillation_EDefibSettingDecayType.TimeBased;
    const float BASE_SHOCK_SUCCESS_CHANCE_TIMEBASED = 0.90;
    const float MIN_SHOCK_SUCCESS_CHANCE_TIMEBASED = 0.0;
    const bool REVIVE_BONUS_DECAY_TIMEBASED = true;
    const float REVIVE_BONUS_DECAY_RATE_TIMEBASED = 480.0;
    const ACE_Medical_Defibrillation_EDefibSettingDecayType REVIVE_BONUS_DECAY_FORMULA_TIMEBASED = ACE_Medical_Defibrillation_EDefibSettingDecayType.TimeBased;
    const float BASE_REVIVE_BONUS_TIMEBASED = 0.25;
    const float MAX_TOTAL_REVIVE_BONUS_TIMEBASED = 0.9;

    //------------------------------------------------------------------------------------------------
    override void Init()
    {
        ApplyPreset();
        // FixMissingValues(); // DISABLED: Due to engine limitation (feedback.bistudio.com/T184217),
        // value types (bool, int, float) cannot be distinguished from user omission vs default values.
        // Both cases result in false/0, making null validation impossible without sentinel values.
        // Mission makers must explicitly define all settings in their headers.
        ValidateSettings();
    }
    
    //------------------------------------------------------------------------------------------------
    // DISABLED - See Init() comment for explanation
    // void FixMissingValues()
    // {
    //     if (!m_bEnabled)
    //         m_bEnabled = true;
    //     
    //     if (!m_bDefibIsMandatory)
    //         m_bDefibIsMandatory = false;
    //     
    //     if (!m_fAED_AnalysisDuration)
    //         m_fAED_AnalysisDuration = AED_ANALYSIS_DURATION_REALISTIC;
    //     
    //     if (!m_fAED_CPRCooldownDuration)
    //         m_fAED_CPRCooldownDuration = AED_CPR_COOLDOWN_DURATION_REALISTIC;
    //     
    //     if (!m_bAED_PlayCPRPacingBeats)
    //         m_bAED_PlayCPRPacingBeats = AED_PLAY_CPR_PACING_BEATS_REALISTIC;
    //     
    //     if (!m_bPunishSpamShocks)
    //         m_bPunishSpamShocks = PUNISH_SPAM_SHOCKS_REALISTIC;
    //     
    //     if (!m_fPunishSpamShocksTimeout)
    //         m_fPunishSpamShocksTimeout = PUNISH_SPAM_SHOCKS_COOLDOWN_REALISTIC;
    //     
    //     if (!m_bShockChanceDecay)
    //         m_bShockChanceDecay = SHOCK_CHANCE_DECAY_REALISTIC;
    //     
    //     if (!m_fShockSuccessDecayRate)
    //         m_fShockSuccessDecayRate = SHOCK_SUCCESS_DECAY_RATE_REALISTIC;
    //     
    //     if (!m_fBaseShockSuccessChance)
    //         m_fBaseShockSuccessChance = BASE_SHOCK_SUCCESS_CHANCE_REALISTIC;
    //     
    //     if (!m_fMinShockSuccessChance && m_fMinShockSuccessChance != 0)
    //         m_fMinShockSuccessChance = MIN_SHOCK_SUCCESS_CHANCE_REALISTIC;
    //     
    //     if (!m_bReviveBonusDecay)
    //         m_bReviveBonusDecay = REVIVE_BONUS_DECAY_REALISTIC;
    //     
    //     if (!m_fReviveBonusDecayRate)
    //         m_fReviveBonusDecayRate = REVIVE_BONUS_DECAY_RATE_REALISTIC;
    //     
    //     if (!m_fBaseReviveBonus)
    //         m_fBaseReviveBonus = BASE_REVIVE_BONUS_REALISTIC;
    //     
    //     if (!m_fMaxTotalReviveBonus)
    //         m_fMaxTotalReviveBonus = MAX_TOTAL_REVIVE_BONUS_REALISTIC;
    // }
    
    //------------------------------------------------------------------------------------------------
    void ValidateSettings()
    {
        m_fAED_AnalysisDuration = Math.Max(m_fAED_AnalysisDuration, 3.0);
        m_fAED_CPRCooldownDuration = Math.Max(m_fAED_CPRCooldownDuration, 0);
        m_fPunishSpamShocksTimeout = Math.Max(m_fPunishSpamShocksTimeout, 0);
        
        if (m_eShockDecayFormula == ACE_Medical_Defibrillation_EDefibSettingDecayType.TimeBased)
        {
            m_fShockSuccessDecayRate = Math.Max(m_fShockSuccessDecayRate, 1.0);
        }
        else if (m_eShockDecayFormula == ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential)
        {
            m_fShockSuccessDecayRate = Math.Clamp(m_fShockSuccessDecayRate, 0.01, 1.0);
        }
        else if (m_eShockDecayFormula == ACE_Medical_Defibrillation_EDefibSettingDecayType.Linear)
        {
            m_fShockSuccessDecayRate = Math.Max(m_fShockSuccessDecayRate, 0);
        }
        else
        {
            m_fShockSuccessDecayRate = Math.Max(m_fShockSuccessDecayRate, 0.01);
        }
        
        if (m_eReviveBonusDecayFormula == ACE_Medical_Defibrillation_EDefibSettingDecayType.TimeBased)
        {
            m_fReviveBonusDecayRate = Math.Max(m_fReviveBonusDecayRate, 1.0);
        }
        else if (m_eReviveBonusDecayFormula == ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential)
        {
            m_fReviveBonusDecayRate = Math.Clamp(m_fReviveBonusDecayRate, 0.01, 1.0);
        }
        else if (m_eReviveBonusDecayFormula == ACE_Medical_Defibrillation_EDefibSettingDecayType.Linear)
        {
            m_fReviveBonusDecayRate = Math.Max(m_fReviveBonusDecayRate, 0);
        }
        else
        {
            m_fReviveBonusDecayRate = Math.Max(m_fReviveBonusDecayRate, 0.01);
        }
        
        m_fBaseShockSuccessChance = Math.Clamp(m_fBaseShockSuccessChance, 0.0, 1.0);
        m_fMinShockSuccessChance = Math.Clamp(m_fMinShockSuccessChance, 0.0, m_fBaseShockSuccessChance);
        
        m_fBaseReviveBonus = Math.Clamp(m_fBaseReviveBonus, 0.0, 1.0);
        m_fMaxTotalReviveBonus = Math.Clamp(m_fMaxTotalReviveBonus, m_fBaseReviveBonus, 1.0);
    }
    
    //------------------------------------------------------------------------------------------------
    void ApplyPreset()
    {
        switch (m_eDefibSettingPreset)
        {
            case ACE_Medical_Defibrillation_EDefibSettingPreset.Custom:
                break;
            
            case ACE_Medical_Defibrillation_EDefibSettingPreset.Realistic:
            {
                m_fAED_AnalysisDuration = AED_ANALYSIS_DURATION_REALISTIC;
                m_fAED_CPRCooldownDuration = AED_CPR_COOLDOWN_DURATION_REALISTIC;
                m_bAED_PlayCPRPacingBeats = AED_PLAY_CPR_PACING_BEATS_REALISTIC;
                m_bPunishSpamShocks = PUNISH_SPAM_SHOCKS_REALISTIC;
                m_fPunishSpamShocksTimeout = PUNISH_SPAM_SHOCKS_COOLDOWN_REALISTIC;
                m_bShockChanceDecay = SHOCK_CHANCE_DECAY_REALISTIC;
                m_fShockSuccessDecayRate = SHOCK_SUCCESS_DECAY_RATE_REALISTIC;
                m_eShockDecayFormula = SHOCK_DECAY_FORMULA_REALISTIC;
                m_fBaseShockSuccessChance = BASE_SHOCK_SUCCESS_CHANCE_REALISTIC;
                m_fMinShockSuccessChance = MIN_SHOCK_SUCCESS_CHANCE_REALISTIC;
                m_bReviveBonusDecay = REVIVE_BONUS_DECAY_REALISTIC;
                m_fReviveBonusDecayRate = REVIVE_BONUS_DECAY_RATE_REALISTIC;
                m_eReviveBonusDecayFormula = REVIVE_BONUS_DECAY_FORMULA_REALISTIC;
                m_fBaseReviveBonus = BASE_REVIVE_BONUS_REALISTIC;
                m_fMaxTotalReviveBonus = MAX_TOTAL_REVIVE_BONUS_REALISTIC;
                break;
            }
            
            case ACE_Medical_Defibrillation_EDefibSettingPreset.Casual:
            {
                m_fAED_AnalysisDuration = AED_ANALYSIS_DURATION_CASUAL;
                m_fAED_CPRCooldownDuration = AED_CPR_COOLDOWN_DURATION_CASUAL;
                m_bAED_PlayCPRPacingBeats = AED_PLAY_CPR_PACING_BEATS_CASUAL;
                m_bPunishSpamShocks = PUNISH_SPAM_SHOCKS_CASUAL;
                m_fPunishSpamShocksTimeout = PUNISH_SPAM_SHOCKS_COOLDOWN_CASUAL;
                m_bShockChanceDecay = SHOCK_CHANCE_DECAY_CASUAL;
                m_fShockSuccessDecayRate = SHOCK_SUCCESS_DECAY_RATE_CASUAL;
                m_eShockDecayFormula = SHOCK_DECAY_FORMULA_CASUAL;
                m_fBaseShockSuccessChance = BASE_SHOCK_SUCCESS_CHANCE_CASUAL;
                m_fMinShockSuccessChance = MIN_SHOCK_SUCCESS_CHANCE_CASUAL;
                m_bReviveBonusDecay = REVIVE_BONUS_DECAY_CASUAL;
                m_fReviveBonusDecayRate = REVIVE_BONUS_DECAY_RATE_CASUAL;
                m_eReviveBonusDecayFormula = REVIVE_BONUS_DECAY_FORMULA_CASUAL;
                m_fBaseReviveBonus = BASE_REVIVE_BONUS_CASUAL;
                m_fMaxTotalReviveBonus = MAX_TOTAL_REVIVE_BONUS_CASUAL;
                break;
            }
            
            case ACE_Medical_Defibrillation_EDefibSettingPreset.Hardcore:
            {
                m_fAED_AnalysisDuration = AED_ANALYSIS_DURATION_HARDCORE;
                m_fAED_CPRCooldownDuration = AED_CPR_COOLDOWN_DURATION_HARDCORE;
                m_bAED_PlayCPRPacingBeats = AED_PLAY_CPR_PACING_BEATS_HARDCORE;
                m_bPunishSpamShocks = PUNISH_SPAM_SHOCKS_HARDCORE;
                m_fPunishSpamShocksTimeout = PUNISH_SPAM_SHOCKS_COOLDOWN_HARDCORE;
                m_bShockChanceDecay = SHOCK_CHANCE_DECAY_HARDCORE;
                m_fShockSuccessDecayRate = SHOCK_SUCCESS_DECAY_RATE_HARDCORE;
                m_eShockDecayFormula = SHOCK_DECAY_FORMULA_HARDCORE;
                m_fBaseShockSuccessChance = BASE_SHOCK_SUCCESS_CHANCE_HARDCORE;
                m_fMinShockSuccessChance = MIN_SHOCK_SUCCESS_CHANCE_HARDCORE;
                m_bReviveBonusDecay = REVIVE_BONUS_DECAY_HARDCORE;
                m_fReviveBonusDecayRate = REVIVE_BONUS_DECAY_RATE_HARDCORE;
                m_eReviveBonusDecayFormula = REVIVE_BONUS_DECAY_FORMULA_HARDCORE;
                m_fBaseReviveBonus = BASE_REVIVE_BONUS_HARDCORE;
                m_fMaxTotalReviveBonus = MAX_TOTAL_REVIVE_BONUS_HARDCORE;
                break;
            }
            
            case ACE_Medical_Defibrillation_EDefibSettingPreset.Arcade:
            {
                m_fAED_AnalysisDuration = AED_ANALYSIS_DURATION_ARCADE;
                m_fAED_CPRCooldownDuration = AED_CPR_COOLDOWN_DURATION_ARCADE;
                m_bAED_PlayCPRPacingBeats = AED_PLAY_CPR_PACING_BEATS_ARCADE;
                m_bPunishSpamShocks = PUNISH_SPAM_SHOCKS_ARCADE;
                m_fPunishSpamShocksTimeout = PUNISH_SPAM_SHOCKS_COOLDOWN_ARCADE;
                m_bShockChanceDecay = SHOCK_CHANCE_DECAY_ARCADE;
                m_fShockSuccessDecayRate = SHOCK_SUCCESS_DECAY_RATE_ARCADE;
                m_eShockDecayFormula = SHOCK_DECAY_FORMULA_ARCADE;
                m_fBaseShockSuccessChance = BASE_SHOCK_SUCCESS_CHANCE_ARCADE;
                m_fMinShockSuccessChance = MIN_SHOCK_SUCCESS_CHANCE_ARCADE;
                m_bReviveBonusDecay = REVIVE_BONUS_DECAY_ARCADE;
                m_fReviveBonusDecayRate = REVIVE_BONUS_DECAY_RATE_ARCADE;
                m_eReviveBonusDecayFormula = REVIVE_BONUS_DECAY_FORMULA_ARCADE;
                m_fBaseReviveBonus = BASE_REVIVE_BONUS_ARCADE;
                m_fMaxTotalReviveBonus = MAX_TOTAL_REVIVE_BONUS_ARCADE;
                break;
            }
            
            case ACE_Medical_Defibrillation_EDefibSettingPreset.TimeBased:
            {
                m_fAED_AnalysisDuration = AED_ANALYSIS_DURATION_TIMEBASED;
                m_fAED_CPRCooldownDuration = AED_CPR_COOLDOWN_DURATION_TIMEBASED;
                m_bAED_PlayCPRPacingBeats = AED_PLAY_CPR_PACING_BEATS_TIMEBASED;
                m_bPunishSpamShocks = PUNISH_SPAM_SHOCKS_TIMEBASED;
                m_fPunishSpamShocksTimeout = PUNISH_SPAM_SHOCKS_COOLDOWN_TIMEBASED;
                m_bShockChanceDecay = SHOCK_CHANCE_DECAY_TIMEBASED;
                m_fShockSuccessDecayRate = SHOCK_SUCCESS_DECAY_RATE_TIMEBASED;
                m_eShockDecayFormula = SHOCK_DECAY_FORMULA_TIMEBASED;
                m_fBaseShockSuccessChance = BASE_SHOCK_SUCCESS_CHANCE_TIMEBASED;
                m_fMinShockSuccessChance = MIN_SHOCK_SUCCESS_CHANCE_TIMEBASED;
                m_bReviveBonusDecay = REVIVE_BONUS_DECAY_TIMEBASED;
                m_fReviveBonusDecayRate = REVIVE_BONUS_DECAY_RATE_TIMEBASED;
                m_eReviveBonusDecayFormula = REVIVE_BONUS_DECAY_FORMULA_TIMEBASED;
                m_fBaseReviveBonus = BASE_REVIVE_BONUS_TIMEBASED;
                m_fMaxTotalReviveBonus = MAX_TOTAL_REVIVE_BONUS_TIMEBASED;
                break;
            }
        }
    }
}