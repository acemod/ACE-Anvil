[BaseContainerProps()]
//Kelvin to Celsius is always 273 degrees in this mod. No exceptions.
class ACE_Medical_Temperature_Settings : ACE_ModSettings
{	
	[Attribute(defvalue: "0.7", desc: "How affected are buildings from the outdoors? 0 = perfectly insulated", params: "0 1")]
	float m_fIndoorInsulation;
	
	[Attribute(defvalue: "0.5", desc: "How affected are  vehicles from the outdoors? 0 = perfectly insulated", params: "0 1")]
	float m_fVehicleInsulation;
	
	[Attribute(defvalue: "50", desc: "Daily max humidity of your map, in %. This controls how fast wet clothes dry.", params: "0 100")]
	float m_fDailyPeakHumidity;
	
	[Attribute(defvalue: "310.15", desc: "Core temperature of a person upon spawning, K", params: "0 1000")]
	float m_fDefaultCoreTemperature;
	
	[Attribute(defvalue: "200", desc: "Effectiveness of core natural heating. Increase this for stronger natural heating. Millidegrees per second.", params: "0 10000")]
	float m_fNaturalCoreHeating;
	
	[Attribute(defvalue: "150", desc: "Effectiveness of core temperature management per second. Increase this for stronger natural heat management. Millidegrees per second.", params: "0 10000")]
	float m_fCoreHeatEfficacy;
	
	[Attribute(defvalue: "100", desc: "Effectiveness of the heat pack item. Increase this for a stronger heating effect per pack. Millidegrees per second.", params: "0 10000")]
	float m_fHeatpackHeating;
	
	
	[Attribute(defvalue:"1", desc:"Do clothing insulation calculations? If false, will default to value set below, regardless of clothing.")]
	bool m_bDoClothingInsulation;
	
	[Attribute(defvalue: "0.005", desc: "Insulation value for a person with 1kg of clothing.", params: "0 1")]
	float m_fDefaultInsulationScore;
	
	
	
	[Attribute(defvalue: "315.0", desc: "Core temperature at which a patient will die, K", params: "0 1000")]
	float m_fLethalTemperature;
	
	[Attribute(defvalue: "10", desc: "How strong is windchill, approx millidegrees / (m/s) of windspeed", params: "0 100")]
	float m_fWindchillPower;
	
	
//	[Attribute(defvalue: "5", desc: "How heavily weighted should brightness influence someone at night?", params: "0 100")]
//	float m_fNightLightPower;
	
	
	
	
	
	

}