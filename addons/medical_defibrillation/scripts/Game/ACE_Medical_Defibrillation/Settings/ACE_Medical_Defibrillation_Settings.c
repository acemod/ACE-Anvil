//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Medical_Defibrillation_Defibrillation_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "true", desc: "Enables the defibrillation system.")]
	bool m_bEnabled;
	
	[Attribute(defvalue: "false", desc: "Defibrillation is mandatory for revive.")]
	bool m_bDefibIsMandatory;
	
	[Attribute(defvalue: "10.0", desc: "Time (s) spent stunned when touching patient during shock.", params: "0 inf")]
	float m_fContactShockStunDuration;
	
	//------------------------------------------------------------------------------------------------
	override void Init()
	{
		m_fContactShockStunDuration = m_fContactShockStunDuration * 1000;
	}
}