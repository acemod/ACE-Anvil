//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Medical_Defibrillation_Settings : ACE_ModSettings
{
    // Core Settings
    [Attribute(defvalue: "true", desc: "Enables the defibrillation system.", category: "Core Settings")]
    bool m_bEnabled;
    
    [Attribute(defvalue: "false", desc: "Defibrillation is mandatory for revive.", category: "Core Settings")]
    bool m_bDefibIsMandatory;
    
	[Attribute(defvalue: "1", uiwidget: UIWidgets.ComboBox, desc: "Uses a preset and ignores and custom settings below.", enums: ParamEnumArray.FromEnum(ACE_Medical_Defibrillation_EDefibSettingPreset), category: "Core Settings")]
    ACE_Medical_Defibrillation_EDefibSettingPreset m_eDefibSettingPreset;

	// Shock Success
	[Attribute(defvalue: "true", desc: "Fails to deliever a successful shock if shocks are too close together.", category: "Custom Shock Settings")]
	bool m_bPunishSpamShocks;
	
	[Attribute(defvalue: "108", desc: "Seconds since last shock to allow for successful shock with no time interval punishment.", category: "Custom Shock Settings")]
	float m_fPunishSpamShocksTimeout;
	
	[Attribute(defvalue: "true", desc: "Shock chance decays with rapid subsequent shocks.", category: "Custom Shock Settings")]
	bool m_bShockChanceDecay;
	
	[Attribute(defvalue: "0.75", desc: "Shock success decay rate.", category: "Custom Shock Settings")]
	float m_fShockSuccessDecayRate;
	
	[Attribute(defvalue: "0", uiwidget: UIWidgets.ComboBox, desc: "Decay formula to apply to the success rate of subsequent shocks.", enums: ParamEnumArray.FromEnum(ACE_Medical_Defibrillation_EDefibSettingDecayType), category: "Custom Shock Settings")]
	ACE_Medical_Defibrillation_EDefibSettingDecayType m_eShockDecayFormula;

	[Attribute(defvalue: "0.9", desc: "Maximum shock success chance.", params: "0, 1", category: "Custom Shock Settings")]
	float m_fMaxShockSuccessChance;
	
	[Attribute(defvalue: "0", desc: "Mininum shock success chance.", params: "0, 1", category: "Custom Shock Settings")]
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
	const bool  PUNISH_SPAM_SHOCKS_REALISTIC = true;
	const float PUNISH_SPAM_SHOCKS_COOLDOWN_REALISTIC = 120 * 0.9;
	const bool 	SHOCK_CHANCE_DECAY_REALISTIC = true;
	const float SHOCK_CHANCE_DECAY_RATE_REALISTIC = 0.75;
	const ACE_Medical_Defibrillation_EDefibSettingDecayType SHOCK_SUCCESS_DECAY_FORMULA_REALISTIC = ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential;
	const float MIN_SHOCK_SUCCESS_CHANCE_REALISTIC = 0;
	const float MAX_SHOCK_SUCCESS_CHANCE_REALISTIC = 90;
	const bool 	REVIVE_BONUS_DECAY_REALISTIC = true;
	const float SHOCK_BONUS_DECAY_RATE_REALISTIC = 0.75;
	const ACE_Medical_Defibrillation_EDefibSettingDecayType SHOCK_BONUS_DECAY_FORMULA_REALISTIC = ACE_Medical_Defibrillation_EDefibSettingDecayType.Exponential;
	const float MAX_REVIVE_BONUS_REALISTIC = 25;
	const float MIN_REVIVE_BONUS_REALISTIC = 0;
    
    // Casual Preset (2)
	const bool  PUNISH_SPAM_SHOCKS_CASUAL = true;
	const float PUNISH_SPAM_SHOCKS_COOLDOWN_CASUAL = 120 * 0.5;
	const bool 	SHOCK_CHANCE_DECAY_CASUAL = true;
	const float SHOCK_CHANCE_DECAY_RATE_CASUAL = 0.15;
	const ACE_Medical_Defibrillation_EDefibSettingDecayType SHOCK_SUCCESS_DECAY_FORMULA_CASUAL = ACE_Medical_Defibrillation_EDefibSettingDecayType.Linear;
	const float MIN_SHOCK_SUCCESS_CHANCE_CASUAL = 0;
	const float MAX_SHOCK_SUCCESS_CHANCE_CASUAL = 90;
	const bool 	REVIVE_BONUS_DECAY_CASUAL = true;
	const float SHOCK_BONUS_DECAY_RATE_CASUAL = 0.15;
	const ACE_Medical_Defibrillation_EDefibSettingDecayType SHOCK_BONUS_DECAY_FORMULA_CASUAL = ACE_Medical_Defibrillation_EDefibSettingDecayType.Linear;
	const float MAX_REVIVE_BONUS_CASUAL = 60;
	const float MIN_REVIVE_BONUS_CASUAL = 25;

    //------------------------------------------------------------------------------------------------
    override void Init()
    {
        ApplyPreset();
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
				m_bShockChanceDecay = SHOCK_CHANCE_DECAY_REALISTIC;
				m_fShockSuccessDecayRate = SHOCK_CHANCE_DECAY_RATE_REALISTIC;
				m_fPunishSpamShocksTimeout = PUNISH_SPAM_SHOCKS_COOLDOWN_REALISTIC;
				m_eShockDecayFormula = SHOCK_SUCCESS_DECAY_FORMULA_REALISTIC;
				m_fMaxShockSuccessChance = MAX_SHOCK_SUCCESS_CHANCE_REALISTIC;
				m_fMinShockSuccessChance = MIN_SHOCK_SUCCESS_CHANCE_REALISTIC;
				
				m_bReviveBonusDecay = REVIVE_BONUS_DECAY_REALISTIC;
				m_fShockBonusDecayRate = SHOCK_BONUS_DECAY_RATE_REALISTIC;
				m_fShockBonusDecayRate = SHOCK_BONUS_DECAY_FORMULA_REALISTIC;
				m_fMaxReviveBonus = MAX_REVIVE_BONUS_REALISTIC;
				m_fMinReviveBonus = MIN_REVIVE_BONUS_REALISTIC;
				break;
			}
                
            case ACE_Medical_Defibrillation_EDefibSettingPreset.Casual:
			{
				m_bShockChanceDecay = SHOCK_CHANCE_DECAY_CASUAL;
				m_fShockSuccessDecayRate = SHOCK_CHANCE_DECAY_RATE_CASUAL;
				m_fPunishSpamShocksTimeout = PUNISH_SPAM_SHOCKS_COOLDOWN_CASUAL;
				m_eShockDecayFormula = SHOCK_SUCCESS_DECAY_FORMULA_CASUAL;
				m_fMaxShockSuccessChance = MAX_SHOCK_SUCCESS_CHANCE_CASUAL;
				m_fMinShockSuccessChance = MIN_SHOCK_SUCCESS_CHANCE_CASUAL;
				
				m_bReviveBonusDecay = REVIVE_BONUS_DECAY_CASUAL;
				m_fShockBonusDecayRate = SHOCK_BONUS_DECAY_RATE_CASUAL;
				m_fShockBonusDecayRate = SHOCK_BONUS_DECAY_FORMULA_CASUAL;
				m_fMaxReviveBonus = MAX_REVIVE_BONUS_CASUAL;
				m_fMinReviveBonus = MIN_REVIVE_BONUS_CASUAL;
				break;
			}
        }
    }
}