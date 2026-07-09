//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Weather_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "true", desc: "Whether advanced simulation of air temperature should be run.")]
	bool m_bAirTemperatureSimulationEnabled;
}
