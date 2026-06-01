[BaseContainerProps()]
//Kelvin to Celsius is always 273 degrees in this mod. No exceptions.
class ACE_Medical_Temperature_Settings : ACE_ModSettings
{
	
	[Attribute(defvalue: "283.0", desc: "Daily low temperature of your map, at sea level, K", params: "0 1000")]
	float m_fDailyLowTemperature;
	
	
	
	
	[Attribute(defvalue: "298.0", desc: "Daily high temperature of your map, at sea level, K. Set this value high for a powerful beating sun at midday", params: "0 1000")]
	float m_fDailyHighTemperature;
	
	[Attribute(defvalue: "307.0", desc: "Core temperature of a person upon spawnin", params: "0 1000")]
	float m_fDefaultCoreTemperature;
	
	[Attribute(defvalue: "400", desc: "Effectiveness of core natural heating. Increase this for stronger natural heating. Millidegrees per second.", params: "0 10000")]
	float m_fNaturalCoreHeating;
	
	[Attribute(defvalue: "220", desc: "Effectiveness of the heat pack item. Increase this for a stronger heating effect per pack. Millidegrees per second.", params: "0 10000")]
	float m_fHeatpackHeating;
	
	[Attribute(defvalue: "150", desc: "Effectiveness of core temperature management per second. Increase this for stronger natural heat management. Millidegrees per second.", params: "0 10000")]
	float m_fCoreHeatEfficacy;
	
	
	[Attribute(defvalue: "1.5", desc: "How powerful of an affect should wind have on insulation", params: "0 100")]
	float m_fWindchillPower;
	
	[Attribute(defvalue: "0.3", desc: "How much of windchill should still leak through by being indoors? Set a low value if your map has very well insulated buildings.", params: "0 1")]
	float m_fIndoorLeakage;
	
	[Attribute(defvalue: "5", desc: "How heavily weighted should brightness influence someone at night?", params: "0 100")]
	float m_fNightLightPower;
	
	[Attribute(defvalue: "315.0", desc: "Core temperature at which a patient will die, K", params: "0 1000")]
	float m_fLethalTemperature;
	
	
	//[Attribute(defvalue: "true", desc: "Enables temperature", params: "0 inf")]
	//bool m_bPaCO2Enabled;

}