modded class ACE_Medical_VitalStateThresholdsConfig : ScriptAndConfig
{
	[Attribute(desc: "A temperature value below this will transition a player to this state", params: "0 1000")]
	float m_fTemperatureThreshold;
}