//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_Medical_EDrugType, "m_eType")]
class ACE_Medical_PharmacodynamicsConfig
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, desc: "Type of the medication", enums: ParamEnumArray.FromEnum(ACE_Medical_EDrugType))]
	ACE_Medical_EDrugType m_eType;
	
	[Attribute(desc: "Concentration at which the half maximum effect is reached [nM]")]
	float m_fEC50;
	
	[Attribute(defvalue: "1", desc: "Hill coefficient", params: "0 inf")]
	float m_fGamma;
	
	[Attribute(desc: "Configs for synergistic effects on this drug effect")]
	ref array<ref ACE_Medical_PharmacodynamicsConfig> m_aSynergistConfigs;
	
	[Attribute(desc: "Configs for antagonistic effects on this drug effect")]
	ref array<ref ACE_Medical_PharmacodynamicsConfig> m_aAntagonistConfigs;
	
	//------------------------------------------------------------------------------------------------
	float ComputeChi(map<ACE_Medical_EDrugType, float> concentrations)
	{
		float concentration
		if (!concentrations.Find(m_eType, concentration))
			return 0;
		
		float chi = concentration / m_fEC50;
		
		if (m_aSynergistConfigs)
		{
			float synergistChiSum = 1;
			
			foreach (ACE_Medical_PharmacodynamicsConfig synergistConfig : m_aSynergistConfigs)
			{
				synergistChiSum += synergistConfig.ComputeChi(concentrations);
			}
			
			chi *= synergistChiSum;
		}

		if (m_aAntagonistConfigs)
		{
			float anatagonistChiSum = 1;
			
			foreach (ACE_Medical_PharmacodynamicsConfig antagonistConfig : m_aAntagonistConfigs)
			{
				anatagonistChiSum += antagonistConfig.ComputeChi(concentrations);
			}
			
			chi /= anatagonistChiSum;
		}

		return Math.Pow(chi, m_fGamma);
	}
}
