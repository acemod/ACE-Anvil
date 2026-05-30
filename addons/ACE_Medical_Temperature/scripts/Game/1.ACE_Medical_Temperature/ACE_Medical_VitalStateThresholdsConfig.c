modded class ACE_Medical_VitalStateThresholdsConfig : ScriptAndConfig
{
	[Attribute(desc: "A temperature value below this will force a player to enter critical", params: "0 1000")]
	float m_fTemperatureThreshold;
}