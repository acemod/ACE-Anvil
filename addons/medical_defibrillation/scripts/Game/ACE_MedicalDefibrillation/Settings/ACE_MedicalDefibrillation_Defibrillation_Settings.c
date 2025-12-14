//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_MedicalDefibrillation_Defibrillation_Settings : ACE_ModSettings
{
	[Attribute(defvalue: "10.0", desc: "Time (s) spent stunned when touching patient during shock.", params: "0 inf")]
	float m_fContactShockStunDuration;
	
	//------------------------------------------------------------------------------------------------
	override void Init()
	{
		m_fContactShockStunDuration = m_fContactShockStunDuration * 1000;
	}
}