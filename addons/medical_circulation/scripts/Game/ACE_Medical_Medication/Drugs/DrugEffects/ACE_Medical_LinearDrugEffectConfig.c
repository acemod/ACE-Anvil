//------------------------------------------------------------------------------------------------
class ACE_Medical_LinearDrugEffectConfig : ACE_Medical_DrugEffectConfig
{
	//------------------------------------------------------------------------------------------------
	//! Sum of chi
	override float ComputeEffect(map<ACE_Medical_EDrugType, float> concentrations)
	{
		float chiSum = 0;
		
		foreach (ACE_Medical_PharmacodynamicsConfig config : m_aDrugConfigs)
		{
			chiSum += config.ComputeChi(concentrations);
		}
		
		return chiSum;
	}
}
