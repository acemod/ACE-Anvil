//------------------------------------------------------------------------------------------------
//! Add SpO2
modded class ACE_Medical_VitalStateThresholdsConfig : ScriptAndConfig
{
	[Attribute(desc: "A SpO2 value below this will trigger a transition to this state", params: "0 100")]
	float m_fSpO2;
}
