//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Medical_Breathing_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "0.15", desc: "Probability of tongue-based airway obstruction in supine position", params: "0 1")]
	float m_fAirwayObstructionChance;
	
	[Attribute(defvalue: "0.8", desc: "Enables tension pneumothorax and hemothorax", params: "0 1")]
	float m_fPneumothoraxChance;
	
	[Attribute(defvalue: "0.1", desc: "Maximum loss of lung volume due to pneumothorax in percentage", params: "0 1")]
	float m_fMaxPneumothoraxScale;
	
	[Attribute(defvalue: "true", desc: "Enables tension pneumothorax and hemothorax")]
	bool m_bAdvancedPneumothoraxEnabled;
	
	[Attribute(defvalue: "true", desc: "Enables simulation of PaCO2 and its influence on respiratory rate", params: "0 inf")]
	bool m_bPaCO2Enabled;
	
	// TO DO: Enable kidney when implemented
	[Attribute(defvalue: "false", desc: "Enables simulation of blood pH and kidney damage")]
	bool m_bKidneyEnabled;
	
	[Attribute(defvalue: "400", desc: "Default tidal volume [ml]", params: "0 inf")]
	float m_fDefaultTidalVolumeML;
	
	[Attribute(defvalue: "5.333", desc: "Default partial pressure of CO2 in blood [kPa]", params: "0 inf")]
	float m_fDefaultPaCO2KPA;
	
	[Attribute(defvalue: "0.21", desc: "Default fraction of inspired O2", params: "0 1")]
	float m_fDefaultFiO2;
	
	[Attribute(defvalue: "12", desc: "Default partial pressure of O2 in blood [kPa]", params: "0 inf")]
	float m_fDefaultPaO2KPA;
	
	[Attribute(defvalue: "2000", desc: "Minimum target ventilation [ml/min]", params: "0 inf")]
	float m_fMinTargetVentilationMLPM;
	
	[Attribute(defvalue: "40", desc: "Maximum respiratory rate [bpm]", params: "0 inf")]
	float m_fMaxRespiratoryRateBPM;
	
	[Attribute(defvalue: "0.00667", desc: "Maximum rate of change of PaCO2 [kPa/s]", precision: 6, params: "0 inf")]
	float m_fMaxPaCO2RateKPAPS;
	
	[Attribute(defvalue: "6.667", desc: "Minimum PaCO2 for increased respiratory rate [kPa]", params: "0 inf")]
	float m_fMinPaCO2RRBoostKPA;
	
	[Attribute(defvalue: "0.0133", desc: "Maximum rate of change of PaO2 [kPa/s]", precision: 6, params: "0 inf")]
	float m_fMaxPaO2RateKPAPS;
}
