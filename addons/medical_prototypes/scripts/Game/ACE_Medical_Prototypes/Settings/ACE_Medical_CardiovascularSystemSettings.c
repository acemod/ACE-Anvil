//------------------------------------------------------------------------------------------------
class ACE_Medical_CardiovascularSystemSettings : ScriptAndConfig
{
	[Attribute(defvalue: "80", desc: "Default heart rate [BPM]", category: "Default Vitals")]
	float m_fDefaultHeartRateBPM;
	
	[Attribute(defvalue: "95", desc: "Default stroke volume [ml]", category: "Default Vitals")]
	float m_fDefaultStrokeVolumeML;
	
	[Attribute(defvalue: "12.443", desc: "Default mean arterial pressure [kPa]", category: "Default Vitals")]
	float m_fDefaultMeanArterialPressureKPA;
	
	[Attribute(defvalue: "5.3329", desc: "Default pulse pressure [kPa]", category: "Default Vitals")]
	float m_fDefaultPulsePressureKPA;
	
	[Attribute(defvalue: "40", desc: "A heart rate below this will results in critical state [BPM]", category: "Critical State")]
	float m_fCriticalHeartRateThresholdLowBPM;
	
	[Attribute(defvalue: "220", desc: "A heart rate above this will results in critical state [BPM]", category: "Critical State")]
	float m_fCriticalHeartRateThresholdHighBPM;
	
	[Attribute(defvalue: "7.11", desc: "An arterial pressure below this will result in critical state [kPa]", category: "Critical State")]
	float m_fCriticalMeanArterialPressureThresholdLowKPA;
	
	[Attribute(defvalue: "29.5", desc: "An arterial pressure above this will result in critical state [kPa]", category: "Critical State")]
	float m_fCriticalMeanArterialPressureThresholdHighKPA;
	
	[Attribute(defvalue: "CLASS_4_HEMORRHAGE", uiwidget: UIWidgets.SearchComboBox, desc: "A blood level below this will result in critical state", enums: ParamEnumArray.FromEnum(ACE_Medical_EBloodState), category: "Critical State")]
	ACE_Medical_EBloodState m_eCriticalBloodLevelThreshold;
	
	[Attribute(desc: "Damage effects caused by cardiac arrest", category: "Cardiac Arrest")]
	ref ACE_Medical_CardiacArrestDamageEffect m_CardiacArrestDamageEffect;
	
	[Attribute(defvalue: "20", desc: "A heart rate below this will results in cardiac arrest [BPM]", category: "Cardiac Arrest")]
	float m_fCardiacArrestHeartRateThresholdLowBPM;
	
	[Attribute(defvalue: "220", desc: "A heart rate above this will results in cardiac arrest [BPM]", category: "Cardiac Arrest")]
	float m_fCardiacArrestHeartRateThresholdHighBPM;
	
	[Attribute(defvalue: "5.21", desc: "An arterial pressure below this will result in cardiac arrest when the heart rate is critical [kPa]", category: "Cardiac Arrest")]
	float m_fCardiacArrestMeanArterialPressureThresholdLowKPA;
	
	[Attribute(defvalue: "29.5", desc: "An arterial pressure above this will result in cardiac arrest [kPa]", category: "Cardiac Arrest")]
	float m_fCardiacArrestMeanArterialPressureThresholdHighKPA;
	
	[Attribute(defvalue: "CLASS_4_HEMORRHAGE", uiwidget: UIWidgets.SearchComboBox, desc: "A blood level below this will result in cardiac arrest", enums: ParamEnumArray.FromEnum(ACE_Medical_EBloodState), category: "Cardiac Arrest")]
	ACE_Medical_EBloodState m_eCardiacArrestBloodLevelThreshold;
	
	[Attribute(defvalue: "15", desc: "Default delay between check for successful CPR [s]", category: "CPR")]
	float m_fCPRSuccessCheckTimeoutS;
	
	[Attribute(defvalue: "0", desc: "Chance for exiting cardiac arrest while performing CPR at class IV hemorrhage", category: "CPR")]
	float m_fCPRSuccessChanceMin;
	
	[Attribute(defvalue: "0.4", desc: "Chance for exiting cardiac arrest while performing CPR at above class II hemorrhage", category: "CPR")]
	float m_fCPRSuccessChanceMax;
	
	[Attribute(defvalue: "CLASS_2_HEMORRHAGE", uiwidget: UIWidgets.SearchComboBox, desc: "A blood level below this will prevent resilience from recovering", enums: ParamEnumArray.FromEnum(ACE_Medical_EBloodState), category: "Resilience")]
	ACE_Medical_EBloodState m_eMinBloodLevelForResilienceRecovery;
	
	[Attribute(defvalue: "0.2", desc: "Maximum resilience recovery scale applied after getting revived. This scale gets multiplied by the scaled health of the brain and will be removed once the patient becomes conscious", category: "Resilience")]
	float m_fMaxRevivalResilienceRecoveryScale;
	
	// Cardiac rhythms settings
	[Attribute(defvalue: "false", desc: "Enables cardiac rhythm and AED systems", category: "Cardiac Rhythms")]
	bool m_bCardiacArrestRhythmEnabled;
		
	[Attribute(defvalue: "0.9", desc: "Defines the likelihood of successful immediate defibrillation, assuming CPR is successfully performed.", category: "Cardiac Rhythms")]
	float m_fCardiacRhythmsSuccessChanceDefibrillationImmediate;
		
	[Attribute(defvalue: "0.5", desc: "Defines the likelihood of successful delayed defibrillation, assuming CPR is successfully performed.", category: "Cardiac Rhythms")]
	float m_fCardiacRhythmsSuccessChanceDefibrillationDelayed;
		
	[Attribute(defvalue: "0.1", desc: "Defines the likelihood of successful revive if in Asystole, assuming CPR is successfully performed.", category: "Cardiac Rhythms")]
	float m_fCardiacRhythmsSuccessChanceAsystole;
	
	[Attribute(defvalue: "0.1", desc: "Reduction in success with subsequent unsuccessful defibrillations", category: "Cardiac Rhythms")]
	float m_fCardiacRhythmsSuccessChanceDefibrillationReduction;
	
	// Values derived from others
	float m_fDefaultSystemicVascularResistance;
	float m_fPulsePressureScale;
	float m_fMeanCriticalCardiacArrestHeartRateThresholdLowBPM;
	
	//------------------------------------------------------------------------------------------------
	//! Calculate derived values
	void ACE_Medical_CardiovascularSystemSettings()
	{
		// Derive default SVR from other values
		m_fDefaultSystemicVascularResistance = m_fDefaultMeanArterialPressureKPA / (m_fDefaultHeartRateBPM * m_fDefaultStrokeVolumeML);
		// MAP multiplied by this value yields PP
		m_fPulsePressureScale = m_fDefaultPulsePressureKPA / m_fDefaultMeanArterialPressureKPA;
		// Mean between critical and cardiac arrest heart rate threshold
		m_fMeanCriticalCardiacArrestHeartRateThresholdLowBPM = 0.5 * (m_fCriticalHeartRateThresholdLowBPM + m_fCardiacArrestHeartRateThresholdLowBPM);
	}
}
