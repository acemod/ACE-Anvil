//------------------------------------------------------------------------------------------------
class ACE_Medical_VitalsStateThresholdsConfig : ScriptAndConfig
{
	[Attribute(desc: "A heart rate below this will trigger a transition to this state [bpm]")]
	float m_fHeartRateLowBPM;
	
	[Attribute(desc: "A heart rate above this will trigger a transition to this state [bpm]")]
	float m_fHeartRateHighBPM;
	
	[Attribute(desc: "A mean arterial pressure below this will trigger a transition to this state [kPa]")]
	float m_fMeanArterialPressureLowKPA;
	
	[Attribute(desc: "A mean arterial pressure above this will trigger a transition to this state [kPa]")]
	float m_fMeanArterialPressureHighKPA;
	
	[Attribute(uiwidget: UIWidgets.SearchComboBox, desc: "Blood volume at this level will trigger a transition to this state", enums: ParamEnumArray.FromEnum(ACE_Medical_EBloodState))]
	ACE_Medical_EBloodState m_eBloodState;
}
