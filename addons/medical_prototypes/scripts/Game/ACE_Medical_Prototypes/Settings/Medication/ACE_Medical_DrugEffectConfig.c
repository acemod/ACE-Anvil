//------------------------------------------------------------------------------------------------
class ACE_Medical_DrugEffectConfig : ScriptAndConfig
{
	[Attribute(desc: "Pharmacodynamics config for all drugs with this effect")]
	ref array<ref ACE_Medical_PharmacodynamicsConfig> m_aDrugConfigs;
	
	//------------------------------------------------------------------------------------------------
	void ApplyEffect(SCR_ChimeraCharacter target, map<ACE_Medical_EDrugType, float> concentrations);
	
	//------------------------------------------------------------------------------------------------
	//! Remove effect from target
	void ClearEffect(SCR_ChimeraCharacter target);

	//------------------------------------------------------------------------------------------------
	float ComputeEffect(map<ACE_Medical_EDrugType, float> concentrations);
	
#ifdef WORKBENCH
	//------------------------------------------------------------------------------------------------
	void OnDiag(SCR_ChimeraCharacter target, float timeSlice);
#endif
}
