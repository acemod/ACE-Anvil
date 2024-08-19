//------------------------------------------------------------------------------------------------
class ACE_Medical_DrugEffectConfig : ScriptAndConfig
{
	[Attribute(desc: "Pharmacodynamics config for all drugs with this effect")]
	ref array<ref ACE_Medical_PharmacodynamicsConfig> m_aDrugConfigs;
	
	//------------------------------------------------------------------------------------------------
	void ApplyEffect(IEntity target, map<ACE_Medical_EDrugType, float> concentrations);

	//------------------------------------------------------------------------------------------------
	float ComputeEffect(map<ACE_Medical_EDrugType, float> concentrations);
}
