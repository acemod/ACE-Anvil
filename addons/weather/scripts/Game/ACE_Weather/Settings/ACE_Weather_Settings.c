//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Weather_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "true", desc: "Whether advanced simulation of air temperature should be run.")]
	bool m_bAirTemperatureSimulationEnabled;
	
	[Attribute(defvalue: "-1", desc: "Interval between wind report updates in seconds (part of the weather report map tool). Negative values disable wind reports.")]
	int m_iMapToolWindReportInterval;
	
	[Attribute(defvalue: "1", desc: "Wind report speed precision in m/s (part of the weather report map tool).")]
	int m_iMapToolWindReportSpeedPrecision;
	
	[Attribute(defvalue: "1", desc: "Wind report direction precision in ° (part of the weather report map tool).")]
	int m_iMapToolWindReportDirectionPrecision;
}
