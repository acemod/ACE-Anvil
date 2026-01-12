//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Medical_Medication_Settings : ACE_ModSettings
{
	[Attribute(desc: "Configurations for all drugs")]
	ref array<ref ACE_Medical_PharmacokineticsConfig> m_aPharmacokineticsConfigs;
	
	[Attribute(desc: "Drug effect configs")]
	ref array<ref ACE_Medical_DrugEffectConfig> m_aPharmacodynamicsConfigs;
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_PharmacokineticsConfig GetPharmacokineticsConfig(ACE_Medical_EDrugType type)
	{
		foreach (ACE_Medical_PharmacokineticsConfig config : m_aPharmacokineticsConfigs)
		{
			if (config.m_eType == type)
				return config;
		}
		
		return null;
	}
}
