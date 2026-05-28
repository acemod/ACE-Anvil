//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Medical_Breathing_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "0.15", desc: "Probability of tongue-based airway obstruction in supine position", params: "0 1")]
	float m_fAirwayObstructionChance;
	
	[Attribute(defvalue: "0.1", desc: "Chance for unconscious patient to vomit. Evaulated once a minute.", params: "0 1")]
	float m_fVomitChancePerMinute;
	
	[Attribute(defvalue: "6", desc: "Minimum delay for vomiting again in minutes", params: "0 inf")]
	float m_fVomitCooldown;
	
	[Attribute(defvalue: "0.51", desc: "Chance for pneumothorax after a critical hit to the chest", params: "0 1")]
	float m_fPneumothoraxChance;
	
	[Attribute(defvalue: "0.5", desc: "Probability for ongoing pneumothorax to deteriorate. Evaluated once a minute.", params: "0 1")]
	float m_fPneumothoraxDeteriorationChancePerMinute;
	
	[Attribute(defvalue: "0.75", desc: "Maximum loss of lung volume due to pneumothorax in percentage", params: "0 1")]
	float m_fMaxPneumothoraxScale;
	
	[Attribute(defvalue: "0.18", desc: "Precentage loss of lung volume per deterioration of pneumothorax", params: "0 1")]
	float m_fPneumothoraxScaleDeteriorationStep;
	
	[Attribute(defvalue: "true", desc: "Enter cardiac arrest when pneumothorax fully deteriorated.")]
	bool m_fPneumothoraxArrestEnabled;
	
	[Attribute(defvalue: "0.15", desc: "Chance for tension pneumothorax after a critical hit to a chest with pneumothorax", params: "0 1")]
	float m_fTensionPneumothoraxChance;
	
	[Attribute(defvalue: "0.05", desc: "Probability for ongoing pneumothorax to deteriorate to tension pneumothorax. Evaluated once a minute.", params: "0 1")]
	float m_fDeteriorateToTensionPneumothoraxChancePerMinute;
	
	[Attribute(defvalue: "14.573", desc: "Default respiratory rate [breaths per minute]", params: "0 inf")]
	float m_fDefaultRespiratoryRateBPM;
	
	[Attribute(defvalue: "40", desc: "Maximum respiratory rate [breaths per minute]", params: "0 inf")]
	float m_fMaxRespiratoryRateBPM;
	
	[Attribute(defvalue: "97.149", desc: "Default value of SpO2", params: "0 100")]
	float m_fDefaultSpO2;
	
	[Attribute(defvalue: "5.21", desc: "Default value of alveolar partial pressure of CO2", params: "0 100")]
	float m_fDefaultPalvCO2KPA;
	
	[Attribute(defvalue: "true", desc: "Enables simulation of PaCO2 and its influence on respiratory rate", params: "0 inf")]
	bool m_bPaCO2Enabled;
	
	// TO DO: Enable kidney when implemented
	[Attribute(defvalue: "false", desc: "Enables simulation of blood pH and kidney damage")]
	bool m_bKidneyEnabled;
}
