//------------------------------------------------------------------------------------------------
class ACE_Medical_HillDrugEffectConfig : ACE_Medical_LinearDrugEffectConfig
{
	[Attribute(defvalue: "1", desc: "Maximum possible effect of this drug")]
	protected float m_fMaxEffect;
	
	//------------------------------------------------------------------------------------------------
	//! Apply hill to sum of chi
	override float ComputeEffect(map<ACE_Medical_EDrugType, float> concentrations)
	{
		return m_fMaxEffect * ACE_Math.Hill(super.ComputeEffect(concentrations));
	}
}
