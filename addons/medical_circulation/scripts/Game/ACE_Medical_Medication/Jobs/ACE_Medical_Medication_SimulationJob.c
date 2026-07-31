//------------------------------------------------------------------------------------------------
//! TO DO: move context to ACE_IFrameJob
class ACE_Medical_Medication_SimulationJob : ACE_IContextFrameJob<ACE_Medical_CharacterContext>
{
	protected static ACE_Medical_Medication_Settings s_pSettings;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_Medication_SimulationJob()
	{
		if (!s_pSettings)
			s_pSettings = ACE_SettingsHelperT<ACE_Medical_Medication_Settings>.GetModSettings();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(float timeSlice)
	{
		array<ACE_Medical_EDrugType> drugs;
		array<ref array<ref ACE_Medical_Dose>> doses;
		m_pContext.m_pMedication.GetMedications(drugs, doses);
		map<ACE_Medical_EDrugType, float> concentrations = ComputeConcentrations(drugs, doses);
		ApplyEffects(concentrations);
		
		// Unregister once no drug is left to manage
		if (drugs.IsEmpty())
			Stop();
	}
	
	//------------------------------------------------------------------------------------------------
	map<ACE_Medical_EDrugType, float> ComputeConcentrations(inout array<ACE_Medical_EDrugType> drugs, inout array<ref array<ref ACE_Medical_Dose>> allDoses, bool deleteExpiredDoses = true)
	{
		map<ACE_Medical_EDrugType, float> concentrations = new map<ACE_Medical_EDrugType, float>();
		
		for (int i = drugs.Count() - 1; i >= 0; i--)
		{
			ACE_Medical_EDrugType drug = drugs[i];
			array<ref ACE_Medical_Dose> doses = allDoses[i];
			concentrations[drug] = ComputeConcentration(drug, doses, deleteExpiredDoses);
			
			// Remove drugs that have no doses left
			if (deleteExpiredDoses && doses.IsEmpty())
			{
				drugs.Remove(i);
				allDoses.Remove(i);
			}
		}
		
		return concentrations;
	}
	
	//------------------------------------------------------------------------------------------------
	float ComputeConcentration(ACE_Medical_EDrugType drug, inout array<ref ACE_Medical_Dose> doses, bool deleteExpiredDoses = true)
	{
		ACE_Medical_PharmacokineticsConfig config = s_pSettings.GetPharmacokineticsConfig(drug);
		float totalConcentration = 0;
		
		// TO DO: Handle IV infusions
		for (int i = doses.Count() - 1; i >= 0; i--)
		{
			ACE_Medical_Dose dose = doses[i];
			float concentration = dose.ComputeConcentration(config);
			
			// Remove doses that have expired
			if (deleteExpiredDoses && dose.IsExpired())
				doses.Remove(i);
			else
				totalConcentration += concentration;
		}
		
		return totalConcentration;
	}
	
	//------------------------------------------------------------------------------------------------
	void ApplyEffects(map<ACE_Medical_EDrugType, float> concentrations)
	{
		foreach (ACE_Medical_DrugEffectConfig config : s_pSettings.m_aPharmacodynamicsConfigs)
		{
			config.ApplyEffect(m_pContext, concentrations);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void CopyFrom(ACE_IFrameJob from)
	{
		super.CopyFrom(from);
	}
}
