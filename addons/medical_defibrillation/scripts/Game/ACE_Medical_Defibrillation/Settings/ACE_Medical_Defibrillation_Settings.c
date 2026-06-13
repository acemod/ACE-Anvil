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
	
	[Attribute(defvalue: "120", params: "0 inf", desc: "Time (s) between AED analysis events where players should perform CPR.", category: "AED Settings")]
    float m_fAED_CPRCooldownDuration;
	
	[Attribute(defvalue: "true", desc: "Plays pacing beats when players should be performing CPR.", category: "AED Settings")]
	bool m_bAED_PlayCPRPacingBeats;
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.ComboBox, desc: "Uses a preset and ignores and custom settings below.", enums: ParamEnumArray.FromEnum(ACE_Medical_Defibrillation_EDefibSettingPreset), category: "Core Settings")]
    ACE_Medical_Defibrillation_EDefibSettingPreset m_eDefibSettingPreset;

	// Shock Success
	[Attribute(defvalue: "true", desc: "Fails to deliever a successful shock if shocks are too close together.", category: "Custom Shock Settings")]
	bool m_bPunishSpamShocks;
	
	[Attribute(defvalue: "108", params: "0 inf", desc: "Seconds since last shock to allow for successful shock with no time interval punishment.", category: "Custom Shock Settings")]
	float m_fPunishSpamShocksTimeout;
	
	[Attribute(defvalue: "true", desc: "Shock chance decays with rapid subsequent shocks.", category: "Custom Shock Settings")]
	bool m_bShockChanceDecay;
	
	[Attribute(defvalue: "0.75", desc: "Shock success decay rate.", category: "Custom Shock Settings")]
	float m_fShockSuccessDecayRate;
	
	[Attribute(defvalue: "0", uiwidget: UIWidgets.ComboBox, desc: "Decay formula to apply to the success rate of subsequent shocks.", enums: ParamEnumArray.FromEnum(ACE_Medical_Defibrillation_EDefibSettingDecayType), category: "Custom Shock Settings")]
	ACE_Medical_Defibrillation_EDefibSettingDecayType m_eShockDecayFormula;

	[Attribute(defvalue: "0.9", desc: "Maximum shock success chance.", params: "0 1", category: "Custom Shock Settings")]
	float m_fMaxShockSuccessChance;
	
	[Attribute(defvalue: "0", desc: "Mininum shock success chance.", params: "0 1", category: "Custom Shock Settings")]
	float m_fMinShockSuccessChance;

	// Revive Bonus Chance
	[Attribute(defvalue: "true", desc: "Revive chance decays with rapid subsequent shocks.", category: "Custom Bonus Settings")]
	bool m_bReviveBonusDecay;
	
	[Attribute(defvalue: "0.75", desc: "Shock bonus decay rate.", category: "Custom Bonus Settings")]
	float m_fShockBonusDecayRate;
	
	[Attribute(defvalue: "0", uiwidget: UIWidgets.ComboBox, desc: "Decay formula to apply to the bonus from subsequent shocks.", enums: ParamEnumArray.FromEnum(ACE_Medical_Defibrillation_EDefibSettingDecayType), category: "Custom Bonus Settings")]
	ACE_Medical_Defibrillation_EDefibSettingDecayType m_eShockBonusDecayFormula;

    [Attribute(defvalue: "0.25", desc: "Maximum revive bonus", params: "0 1", category: "Custom Bonus Settings")]
    float m_fMaxReviveBonus;

    [Attribute(defvalue: "0", desc: "Minimum revive bonus", params: "0 1", category: "Custom Bonus Settings")]
    float m_fMinReviveBonus;
    
    // Realistic Preset (1)
	const float AED_ANALYSIS_DURATION_REALISTIC = 3;
	const float AED_CPR_COOLDOWN_DURATION_REALISTIC = 120;
	const bool 	AED_PLAY_CPR_PACING_BEATS_REALISTIC = true;
	const bool  PUNISH_SPAM_SHOCKS_REALISTIC = true;
	const float PUNISH_SPAM_SHOCKS_COOLDOWN_REALISTIC = 120 * 0.9;
	const bool 	SHOCK_CHANCE_DECAY_REALISTIC = true;
	const float SHOCK_CHANCE_DECAY_RATE_REALISTIC = 0.75;
	const ACE_Medical_Defibrillation_EDefibSettingDecayType SHOCK_SUCCESS_DECAY_FORMULA_REALISTIC = ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential;
	const float MIN_SHOCK_SUCCESS_CHANCE_REALISTIC = 0;
	const float MAX_SHOCK_SUCCESS_CHANCE_REALISTIC = 0.90;
	const bool 	REVIVE_BONUS_DECAY_REALISTIC = true;
	const float SHOCK_BONUS_DECAY_RATE_REALISTIC = 0.75;
	const ACE_Medical_Defibrillation_EDefibSettingDecayType SHOCK_BONUS_DECAY_FORMULA_REALISTIC = ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential;
	const float MAX_REVIVE_BONUS_REALISTIC = 0.25;
	const float MIN_REVIVE_BONUS_REALISTIC = 0.0;
    
    // Casual Preset (2)
	const float AED_ANALYSIS_DURATION_CASUAL = 3;
	const float AED_CPR_COOLDOWN_DURATION_CASUAL = 120;
	const bool 	AED_PLAY_CPR_PACING_BEATS_CASUAL = true;
	const bool  PUNISH_SPAM_SHOCKS_CASUAL = true;
	const float PUNISH_SPAM_SHOCKS_COOLDOWN_CASUAL = 120 * 0.5;
	const bool 	SHOCK_CHANCE_DECAY_CASUAL = true;
	const float SHOCK_CHANCE_DECAY_RATE_CASUAL = 0.15;
	const ACE_Medical_Defibrillation_EDefibSettingDecayType SHOCK_SUCCESS_DECAY_FORMULA_CASUAL = ACE_Medical_Defibrillation_EDefibSettingDecayType.Linear;
	const float MIN_SHOCK_SUCCESS_CHANCE_CASUAL = 0;
	const float MAX_SHOCK_SUCCESS_CHANCE_CASUAL = 0.90;
	const bool 	REVIVE_BONUS_DECAY_CASUAL = true;
	const float SHOCK_BONUS_DECAY_RATE_CASUAL = 0.15;
	const ACE_Medical_Defibrillation_EDefibSettingDecayType SHOCK_BONUS_DECAY_FORMULA_CASUAL = ACE_Medical_Defibrillation_EDefibSettingDecayType.Linear;
	const float MAX_REVIVE_BONUS_CASUAL = 0.60;
	const float MIN_REVIVE_BONUS_CASUAL = 0.25;

    //------------------------------------------------------------------------------------------------
    override void Init()
    {
        ApplyPreset();
		FixMissingValues();
		ValidateSettings();
    }
	
    //------------------------------------------------------------------------------------------------
    void FixMissingValues()
    {
        if (!m_bEnabled)
            m_bEnabled = true;
        
        if (!m_bDefibIsMandatory)
            m_bDefibIsMandatory = false;
        
        if (!m_fAED_AnalysisDuration)
            m_fAED_AnalysisDuration = AED_ANALYSIS_DURATION_REALISTIC;
        
        if (!m_fAED_CPRCooldownDuration)
            m_fAED_CPRCooldownDuration = AED_CPR_COOLDOWN_DURATION_REALISTIC;
        
        if (!m_bAED_PlayCPRPacingBeats)
            m_bAED_PlayCPRPacingBeats = AED_PLAY_CPR_PACING_BEATS_REALISTIC;
        
        if (!m_bPunishSpamShocks)
            m_bPunishSpamShocks = PUNISH_SPAM_SHOCKS_REALISTIC;
        
        if (!m_fPunishSpamShocksTimeout)
            m_fPunishSpamShocksTimeout = PUNISH_SPAM_SHOCKS_COOLDOWN_REALISTIC;
        
        if (!m_bShockChanceDecay)
            m_bShockChanceDecay = SHOCK_CHANCE_DECAY_REALISTIC;
        
        if (!m_fShockSuccessDecayRate)
            m_fShockSuccessDecayRate = SHOCK_CHANCE_DECAY_RATE_REALISTIC;
        
        if (!m_fMaxShockSuccessChance)
            m_fMaxShockSuccessChance = MAX_SHOCK_SUCCESS_CHANCE_REALISTIC;
        
        if (!m_fMinShockSuccessChance && m_fMinShockSuccessChance != 0)
            m_fMinShockSuccessChance = MIN_SHOCK_SUCCESS_CHANCE_REALISTIC;
        
        if (!m_bReviveBonusDecay)
            m_bReviveBonusDecay = REVIVE_BONUS_DECAY_REALISTIC;
        
        if (!m_fShockBonusDecayRate)
            m_fShockBonusDecayRate = SHOCK_BONUS_DECAY_RATE_REALISTIC;
        
        if (!m_fMaxReviveBonus)
            m_fMaxReviveBonus = MAX_REVIVE_BONUS_REALISTIC;
        
        if (!m_fMinReviveBonus && m_fMinReviveBonus != 0)
            m_fMinReviveBonus = MIN_REVIVE_BONUS_REALISTIC;
    }
	
	//------------------------------------------------------------------------------------------------
	void ValidateSettings()
	{
	    m_fAED_AnalysisDuration = Math.Max(m_fAED_AnalysisDuration, 3.0);
	    m_fAED_CPRCooldownDuration = Math.Max(m_fAED_CPRCooldownDuration, 0);
	    m_fPunishSpamShocksTimeout = Math.Max(m_fPunishSpamShocksTimeout, 0);
	    
	    switch (m_eShockDecayFormula)
	    {
	        case ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential:
	            m_fShockSuccessDecayRate = Math.Clamp(m_fShockSuccessDecayRate, 0.01, 1.0);
	            break;
	            
	        case ACE_Medical_Defibrillation_EDefibSettingDecayType.Linear:
	            m_fShockSuccessDecayRate = Math.Max(m_fShockSuccessDecayRate, 0);
	            break;
	            
	        case ACE_Medical_Defibrillation_EDefibSettingDecayType.Reciprocal:
	            m_fShockSuccessDecayRate = Math.Max(m_fShockSuccessDecayRate, 0.01);
	            break;
	            
	        case ACE_Medical_Defibrillation_EDefibSettingDecayType.SCurve:
	            m_fShockSuccessDecayRate = Math.Max(m_fShockSuccessDecayRate, 0.01);
	            break;
	    }
	    
	    switch (m_eShockBonusDecayFormula)
	    {
	        case ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential:
	            m_fShockBonusDecayRate = Math.Clamp(m_fShockBonusDecayRate, 0.01, 1.0);
	            break;
	            
	        case ACE_Medical_Defibrillation_EDefibSettingDecayType.Linear:
	            m_fShockBonusDecayRate = Math.Max(m_fShockBonusDecayRate, 0);
	            break;
	            
	        case ACE_Medical_Defibrillation_EDefibSettingDecayType.Reciprocal:
	            m_fShockBonusDecayRate = Math.Max(m_fShockBonusDecayRate, 0.01);
	            break;
	            
	        case ACE_Medical_Defibrillation_EDefibSettingDecayType.SCurve:
	            m_fShockBonusDecayRate = Math.Max(m_fShockBonusDecayRate, 0.01);
	            break;
	    }
	    
	    m_fMaxShockSuccessChance = Math.Clamp(m_fMaxShockSuccessChance, 0.0, 1.0);
	    m_fMinShockSuccessChance = Math.Clamp(m_fMinShockSuccessChance, 0.0, m_fMaxShockSuccessChance);
	    
	    m_fMaxReviveBonus = Math.Clamp(m_fMaxReviveBonus, 0.0, 1.0);
	    m_fMinReviveBonus = Math.Clamp(m_fMinReviveBonus, 0.0, m_fMaxReviveBonus);
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
			    m_bShockChanceDecay = SHOCK_CHANCE_DECAY_REALISTIC;
			    m_fShockSuccessDecayRate = SHOCK_CHANCE_DECAY_RATE_REALISTIC;
			    m_fPunishSpamShocksTimeout = PUNISH_SPAM_SHOCKS_COOLDOWN_REALISTIC;
			    m_eShockDecayFormula = SHOCK_SUCCESS_DECAY_FORMULA_REALISTIC;
			    m_fMaxShockSuccessChance = MAX_SHOCK_SUCCESS_CHANCE_REALISTIC;
			    m_fMinShockSuccessChance = MIN_SHOCK_SUCCESS_CHANCE_REALISTIC;
			    
			    m_bReviveBonusDecay = REVIVE_BONUS_DECAY_REALISTIC;
			    m_fShockBonusDecayRate = SHOCK_BONUS_DECAY_RATE_REALISTIC;
			    m_eShockBonusDecayFormula = SHOCK_BONUS_DECAY_FORMULA_REALISTIC;
			    m_fMaxReviveBonus = MAX_REVIVE_BONUS_REALISTIC;
			    m_fMinReviveBonus = MIN_REVIVE_BONUS_REALISTIC;
			    break;
			}
			
			case ACE_Medical_Defibrillation_EDefibSettingPreset.Casual:
			{
			    m_fAED_AnalysisDuration = AED_ANALYSIS_DURATION_CASUAL;
			    m_fAED_CPRCooldownDuration = AED_CPR_COOLDOWN_DURATION_CASUAL;
			    m_bAED_PlayCPRPacingBeats = AED_PLAY_CPR_PACING_BEATS_CASUAL;
			    m_bPunishSpamShocks = PUNISH_SPAM_SHOCKS_CASUAL;
			    m_bShockChanceDecay = SHOCK_CHANCE_DECAY_CASUAL;
			    m_fShockSuccessDecayRate = SHOCK_CHANCE_DECAY_RATE_CASUAL;
			    m_fPunishSpamShocksTimeout = PUNISH_SPAM_SHOCKS_COOLDOWN_CASUAL;
			    m_eShockDecayFormula = SHOCK_SUCCESS_DECAY_FORMULA_CASUAL;
			    m_fMaxShockSuccessChance = MAX_SHOCK_SUCCESS_CHANCE_CASUAL;
			    m_fMinShockSuccessChance = MIN_SHOCK_SUCCESS_CHANCE_CASUAL;
			    
			    m_bReviveBonusDecay = REVIVE_BONUS_DECAY_CASUAL;
			    m_fShockBonusDecayRate = SHOCK_BONUS_DECAY_RATE_CASUAL;
			    m_eShockBonusDecayFormula = SHOCK_BONUS_DECAY_FORMULA_CASUAL;
			    m_fMaxReviveBonus = MAX_REVIVE_BONUS_CASUAL;
			    m_fMinReviveBonus = MIN_REVIVE_BONUS_CASUAL;
			    break;
			}
        }
    }
}