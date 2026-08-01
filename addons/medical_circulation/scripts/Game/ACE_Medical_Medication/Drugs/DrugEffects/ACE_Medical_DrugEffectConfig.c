//------------------------------------------------------------------------------------------------
class ACE_Medical_DrugEffectConfig : ScriptAndConfig
{
	[Attribute(desc: "Pharmacodynamics config for all drugs with this effect")]
	ref array<ref ACE_Medical_PharmacodynamicsConfig> m_aDrugConfigs;
	
	//------------------------------------------------------------------------------------------------
	void ApplyEffect(ACE_Medical_CharacterContext targetContext, map<ACE_Medical_EDrugType, float> concentrations, float timeSlice);
	
	//------------------------------------------------------------------------------------------------
	//! Remove effect from target
	void ClearEffect(ACE_Medical_CharacterContext targetContext);

	//------------------------------------------------------------------------------------------------
	protected float ComputeEffect(map<ACE_Medical_EDrugType, float> concentrations);
	
#ifdef ENABLE_DIAG
	//------------------------------------------------------------------------------------------------
	void OnDiag(ACE_Medical_CharacterContext targetContext, float timeSlice);
#endif
}
