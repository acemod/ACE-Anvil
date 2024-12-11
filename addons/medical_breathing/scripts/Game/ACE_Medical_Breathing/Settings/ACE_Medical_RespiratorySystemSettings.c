//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true)]
class ACE_Medical_RespiratorySystemSettings
{
	[Attribute(defvalue: "true", desc: "Enables simulation of PaCO2 and its influence on respiratory rate")]
	bool m_bPaCO2Enabled;
	
	// TO DO: Enable kidney when implemented
	[Attribute(defvalue: "false", desc: "Enables simulation of blood pH and kidney damage")]
	bool m_bKidneyEnabled;
	
	[Attribute(defvalue: "0.3", desc: "Enables tension pneumothorax and hemothorax")]
	bool m_fPneumothoraxChance;
	
	[Attribute(defvalue: "true", desc: "Enables tension pneumothorax and hemothorax")]
	bool m_bAdvancedPneumothoraxEnabled;
	
	[Attribute(defvalue: "400", desc: "Default tidal volume [ml]")]
	float m_fDefaultTidalVolumeML;
	
	[Attribute(defvalue: "5.333", desc: "Default partial pressure of CO2 in blood [kPa]")]
	float m_fDefaultPaCO2KPA;
	
	[Attribute(defvalue: "0.21", desc: "Default fraction of inspired O2")]
	float m_fDefaultFiO2;
	
	[Attribute(defvalue: "12", desc: "Default partial pressure of O2 in blood [kPa]")]
	float m_fDefaultPaO2KPA;
	
	[Attribute(defvalue: "7.4", desc: "Default pH of blood")]
	float m_fDefaultpH;
	
	[Attribute(defvalue: "25.803", desc: "Default dissociation constant of O2 for hemoglobin")]
	float m_fDefaultHemoglobinKd;
	
	[Attribute(defvalue: "2000", desc: "Minimum target ventilation [ml/min]")]
	float m_fMinTargetVentilationMLPM;
	
	[Attribute(defvalue: "40", desc: "Maximum respiratory rate [bpm]")]
	float m_fMaxRespiratoryRateBPM;
	
	[Attribute(defvalue: "0.00667", desc: "Maximum rate of change of PaCO2 [kPa/s]", precision: 6)]
	float m_fMaxPaCO2RateKPAPS;
	
	[Attribute(defvalue: "6.667", desc: "Minimum PaCO2 for increased respiratory rate [kPa]")]
	float m_fMinPaCO2RRBoostKPA;
	
	[Attribute(defvalue: "0.0133", desc: "Maximum rate of change of PaO2 [kPa/s]", precision: 6)]
	float m_fMaxPaO2RateKPAPS;
	
	[Attribute(defvalue: "6.1", desc: "pKa of carbonic acid")]
	float m_fpKaCarbonicAcid;
		
	[Attribute(defvalue: "300", desc: "Maximum volume of pneumothorax [ml]")]
	float m_fMaxPneumothoraxVolumeML;
}
