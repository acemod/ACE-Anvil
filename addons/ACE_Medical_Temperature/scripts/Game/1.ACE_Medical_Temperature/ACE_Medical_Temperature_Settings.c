[BaseContainerProps()]
//Kelvin to Celsius is always 273 degrees in this mod. No exceptions.
class ACE_Medical_Temperature_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "true", desc: "Enable updating temperature. If disabled, temperature will remain fixed at the spawning value.", params: "0 inf")]
	bool m_bEnableTemperature;
	
	[Attribute(defvalue: "293.0", desc: "Daily low temperature of your map, at sea level, K", params: "0 1000")]
	float m_fAmbientTemperature;
	
	[Attribute(defvalue: "304.0", desc: "Core temperature of a person upon spawnin", params: "0 1000")]
	float m_fDefaultCoreTemperature;
	
	[Attribute(defvalue: "315.0", desc: "Core temperature at which a patient will die, K", params: "0 1000")]
	float m_fLethalTemperature;
	
	
	//[Attribute(defvalue: "true", desc: "Enables temperature", params: "0 inf")]
	//bool m_bPaCO2Enabled;

}