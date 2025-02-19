//------------------------------------------------------------------------------------------------
//! Updates to vitals are mostly server side right now.
//! Clients can request values for vitals via ACE_Medical_NetworkComponent
//! TO DO: Inherit from ACE_Medical_BaseSystem once systems support inheritance
class ACE_Medical_MedicationSystem : ACE_Medical_BaseSystem2
{
	protected ACE_Medical_MedicationSystemSettings m_Settings;
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(false);
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		super.OnInit();
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (settings && settings.m_MedicationSystem)
			m_Settings = settings.m_MedicationSystem;
	}
	
	//------------------------------------------------------------------------------------------------
	override void Update(IEntity entity, float timeSlice)
	{
		super.Update(entity, timeSlice);
		
		ACE_Medical_MedicationComponent component = ACE_Medical_MedicationComponent.Cast(entity.FindComponent(ACE_Medical_MedicationComponent));
		if (!component)
			return;
				
		array<ACE_Medical_EDrugType> drugs;
		array<ref array<ref ACE_Medical_Dose>> doses;
		component.GetMedications(drugs, doses);
		map<ACE_Medical_EDrugType, float> concentrations = ComputeConcentrations(drugs, doses);
		ApplyEffects(entity, concentrations);
		
		// Unregister once no drug is left to manage
		if (drugs.IsEmpty())
			Unregister(entity);
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
		ACE_Medical_PharmacokineticsConfig config = m_Settings.GetPharmacokineticsConfig(drug);
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
	void ApplyEffects(IEntity target, map<ACE_Medical_EDrugType, float> concentrations)
	{
		foreach (ACE_Medical_DrugEffectConfig config : m_Settings.m_aPharmacodynamicsConfigs)
		{
			config.ApplyEffect(target, concentrations);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clear body of all drugs and effects
	override void OnFullHeal(IEntity entity)
	{
		super.OnFullHeal(entity);
		
		ACE_Medical_MedicationComponent component = ACE_Medical_MedicationComponent.Cast(entity.FindComponent(ACE_Medical_MedicationComponent));
		if (component)
			component.Clear();
		
		Unregister(entity);
	}
	
#ifdef WORKBENCH
	//------------------------------------------------------------------------------------------------
	//! Show medication for target entity
	override protected void OnDiag(float timeSlice)
	{
		super.OnDiag(timeSlice);
		
		IEntity target;
		string targetType;
		if (!GetDiagTarget(target, targetType))
			return;
		
		ACE_Medical_MedicationComponent component = ACE_Medical_MedicationComponent.Cast(target.FindComponent(ACE_Medical_MedicationComponent));
		if (!component)
			return;

		DbgUI.Begin(string.Format("ACE_Medical_MedicationSystem (%1)", targetType), 700);
		DbgUI.Text("Drugs:\n===============================");
		array<ACE_Medical_EDrugType> drugs;
		array<ref array<ref ACE_Medical_Dose>> doses;
		component.GetMedications(drugs, doses);
		
		foreach (ACE_Medical_EDrugType drug, float concentration : ComputeConcentrations(drugs, doses, deleteExpiredDoses: false))
		{
			DbgUI.Text(string.Format("- %1:    %2 nM", SCR_Enum.GetEnumName(ACE_Medical_EDrugType, drug), concentration));
		}
		
		DbgUI.Text("\nEffects:\n===============================");
		
		foreach (ACE_Medical_DrugEffectConfig effect : m_Settings.m_aPharmacodynamicsConfigs)
		{
			effect.OnDiag(target, timeSlice);
		}

		DbgUI.End();
	}
#endif
}
