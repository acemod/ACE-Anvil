//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Medical_Circulation_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "80", desc: "Default heart rate [bpm]", params: "0 inf")]
	float m_fDefaultHeartRateBPM;
	
	[Attribute(defvalue: "95", desc: "Default stroke volume [ml]", params: "0 inf")]
	float m_fDefaultStrokeVolumeML;
	
	[Attribute(defvalue: "12.443", desc: "Default mean arterial pressure [kPa]", params: "0 inf")]
	float m_fDefaultMeanArterialPressureKPA;
	
	[Attribute(defvalue: "5.3329", desc: "Default pulse pressure [kPa]", params: "0 inf")]
	float m_fDefaultPulsePressureKPA;
	
	[Attribute(desc: "Thresholds that lead to unstable vital state")]
	ref ACE_Medical_VitalStateThresholdsConfig m_UnstableThresholds;
	
	[Attribute(desc: "Thresholds that lead to critical vital state")]
	ref ACE_Medical_VitalStateThresholdsConfig m_CriticalThresholds;
	
	[Attribute(desc: "Thresholds that lead to cardiac arrest state")]
	ref ACE_Medical_VitalStateThresholdsConfig m_CardiacArrestThresholds;
	
	[Attribute(desc: "Damage effects caused by cardiac arrest")]
	ref ACE_Medical_CardiacArrestDamageEffect m_CardiacArrestDamageEffect;
	
	[Attribute(defvalue: "false", desc: "AI will die instead of entering cardiac arrest if false")]
	bool m_bCardiacArrestForAIEnabled;
	
	[Attribute(defvalue: "22", desc: "Default delay between check for successful CPR [s]", params: "0 inf")]
	float m_fCPRSuccessCheckTimeoutS;
	
	[Attribute(defvalue: "0", desc: "Chance for exiting cardiac arrest while performing CPR at class IV hemorrhage", params: "0 1")]
	float m_fCPRSuccessChanceMin;
	
	[Attribute(defvalue: "0.4", desc: "Chance for exiting cardiac arrest while performing CPR at above class II hemorrhage", params: "0 1")]
	float m_fCPRSuccessChanceMax;
	
	[Attribute(defvalue: "0.2", desc: "Maximum resilience recovery scale applied after getting revived. This scale gets multiplied by the scaled health of the brain and will be removed once the patient becomes conscious", params: "0 inf")]
	float m_fMaxRevivalResilienceRecoveryScale;
	
	// Values derived from others
	float m_fDefaultSystemicVascularResistance;
	float m_fPulsePressureScale;
	
	//------------------------------------------------------------------------------------------------
	//! Calculate derived values
	void ACE_Medical_Circulation_Settings()
	{
		// Make sure we retain the default settings for subconfigs when left empty
		if (!m_CardiacArrestDamageEffect)
			m_CardiacArrestDamageEffect = new ACE_Medical_CardiacArrestDamageEffect();
		
		// Derive default SVR from other values
		m_fDefaultSystemicVascularResistance = m_fDefaultMeanArterialPressureKPA / (m_fDefaultHeartRateBPM * m_fDefaultStrokeVolumeML);
		// MAP multiplied by this value yields PP
		m_fPulsePressureScale = m_fDefaultPulsePressureKPA / m_fDefaultMeanArterialPressureKPA;
	}
}
