//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true)]
class ACE_Medical_CardiovascularSystemSettings
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
	
	[Attribute(defvalue: "CLASS_3_HEMORRHAGE", uiwidget: UIWidgets.SearchComboBox, desc: "A blood level below this will result in critical state", enums: ParamEnumArray.FromEnum(ACE_Medical_EBloodState), category: "Critical State")]
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
	
	[Attribute(defvalue: "false", desc: "AI will die instead of entering cardiac arrest if false", category: "Cardiac Arrest")]
	bool m_bCardiacArrestForAIEnabled;
	
	[Attribute(defvalue: "22", desc: "Default delay between check for successful CPR [s]", category: "CPR")]
	float m_fCPRSuccessCheckTimeoutS;
	
	[Attribute(defvalue: "0", desc: "Chance for exiting cardiac arrest while performing CPR at class IV hemorrhage", category: "CPR")]
	float m_fCPRSuccessChanceMin;
	
	[Attribute(defvalue: "0.4", desc: "Chance for exiting cardiac arrest while performing CPR at above class II hemorrhage", category: "CPR")]
	float m_fCPRSuccessChanceMax;
	
	[Attribute(defvalue: "CLASS_2_HEMORRHAGE", uiwidget: UIWidgets.SearchComboBox, desc: "A blood level below this will prevent resilience from recovering", enums: ParamEnumArray.FromEnum(ACE_Medical_EBloodState), category: "Resilience")]
	ACE_Medical_EBloodState m_eMinBloodLevelForResilienceRecovery;
	
	[Attribute(defvalue: "0.2", desc: "Maximum resilience recovery scale applied after getting revived. This scale gets multiplied by the scaled health of the brain and will be removed once the patient becomes conscious", category: "Resilience")]
	float m_fMaxRevivalResilienceRecoveryScale;
	
	// Values derived from others
	float m_fDefaultSystemicVascularResistance;
	float m_fPulsePressureScale;
	float m_fMeanCriticalCardiacArrestHeartRateThresholdLowBPM;
	
	//------------------------------------------------------------------------------------------------
	//! Calculate derived values
	void ACE_Medical_CardiovascularSystemSettings()
	{
		// Make sure we retain the default settings for subconfigs when left empty
		if (!m_CardiacArrestDamageEffect)
			m_CardiacArrestDamageEffect = new ACE_Medical_CardiacArrestDamageEffect();
		
		// Derive default SVR from other values
		m_fDefaultSystemicVascularResistance = m_fDefaultMeanArterialPressureKPA / (m_fDefaultHeartRateBPM * m_fDefaultStrokeVolumeML);
		// MAP multiplied by this value yields PP
		m_fPulsePressureScale = m_fDefaultPulsePressureKPA / m_fDefaultMeanArterialPressureKPA;
		// Mean between critical and cardiac arrest heart rate threshold
		m_fMeanCriticalCardiacArrestHeartRateThresholdLowBPM = 0.5 * (m_fCriticalHeartRateThresholdLowBPM + m_fCardiacArrestHeartRateThresholdLowBPM);
	}
}
