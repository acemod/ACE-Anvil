//------------------------------------------------------------------------------------------------
class ACE_Medical_MedicationSystem : GameSystem
{
	[Attribute()]
	protected ref ACE_Medical_Medication_JobScheduler m_pScheduler;
	
	//------------------------------------------------------------------------------------------------
	static ACE_Medical_MedicationSystem GetInstance(ChimeraWorld world)
	{
		return ACE_Medical_MedicationSystem.Cast(world.FindSystem(ACE_Medical_MedicationSystem));
	}
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(false)
			.SetUnique(true)
			.SetLocation(WorldSystemLocation.Server)
			.AddPoint(WorldSystemPoint.BeforeEntitiesCreated);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		m_pScheduler.OnInit(CreateTemplateJob());
		
		ACE_FrameJobSystem jobManager = ACE_FrameJobSystem.GetInstance();
		if (jobManager)
			jobManager.Register(m_pScheduler, ESystemPoint.Frame);
	}
	
	//------------------------------------------------------------------------------------------------
	protected ACE_Medical_Medication_SimulationJob CreateTemplateJob()
	{
		return new ACE_Medical_Medication_SimulationJob();
	}
	
	//------------------------------------------------------------------------------------------------
	void Register(notnull SCR_ChimeraCharacter char)
	{
		m_pScheduler.Register(char);
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(SCR_ChimeraCharacter char)
	{
		m_pScheduler.Unregister(char);
	}
	
#ifdef ENABLE_DIAG
	//------------------------------------------------------------------------------------------------
	//! Show medication for target entity
	override void OnDiag(float timeSlice)
	{
		super.OnDiag(timeSlice);
		
		IEntity target;
		string targetType;
		if (!ACE_DiagTools.GetDiagTargetCharacter(target, targetType))
			return;
		
		ACE_Medical_CharacterContext targetContext = new ACE_Medical_CharacterContext(SCR_ChimeraCharacter.Cast(target));
		ACE_Medical_Medication_SimulationJob job = CreateTemplateJob();
		job.SetContext(targetContext);

		DbgUI.Begin(string.Format("ACE_Medical_MedicationSystem (%1)", targetType), 700);
		DbgUI.Text("Drugs:\n===============================");
		array<ACE_Medical_EDrugType> drugs;
		array<ref array<ref ACE_Medical_Dose>> doses;
		targetContext.m_pMedication.GetMedications(drugs, doses);
		
		foreach (ACE_Medical_EDrugType drug, float concentration : job.ComputeConcentrations(drugs, doses, deleteExpiredDoses: false))
		{
			DbgUI.Text(string.Format("- %1:    %2 nM", SCR_Enum.GetEnumName(ACE_Medical_EDrugType, drug), concentration));
			DbgUI.PlotLive(SCR_Enum.GetEnumName(ACE_Medical_EDrugType, drug), 500, 100, concentration, 100, 100000);
		}
		
		ACE_Medical_Medication_Settings settings = ACE_SettingsHelperT<ACE_Medical_Medication_Settings>.GetModSettings();
		if (!settings)
			return;
		
		DbgUI.Text("\nEffects:\n===============================");
		
		foreach (ACE_Medical_DrugEffectConfig effect : settings.m_aPharmacodynamicsConfigs)
		{
			effect.OnDiag(targetContext, timeSlice);
		}

		float bloodFlowScale = 1;
		if (targetContext.m_pDamageManager)
			bloodFlowScale = targetContext.m_pDamageManager.ACE_Medical_GetBloodFlowScale();

		bool hasInfusions = false;
		for (int doseIdx = 0; doseIdx < doses.Count(); doseIdx++)
		{
			array<ref ACE_Medical_Dose> drugDoses = doses[doseIdx];
			foreach (ACE_Medical_Dose dose : drugDoses)
			{
				ACE_Medical_Infusion infusion = ACE_Medical_Infusion.Cast(dose);
				if (!infusion)
					continue;

				if (!hasInfusions)
				{
					hasInfusions = true;
					DbgUI.Text("\nInfusions:\n===============================");
				}

				float baseRate = infusion.GetInfusionRate();
				float effectiveRate = baseRate * bloodFlowScale;
				float elapsed = infusion.GetElapsedTime();
				float duration = infusion.GetInfusionDuration();
				float remaining = Math.Max(0, duration - elapsed);
				string drugName = SCR_Enum.GetEnumName(ACE_Medical_EDrugType, drugs[doseIdx]);
				DbgUI.Text(string.Format("- %1: base %2 nM/s, effective %3 nM/s, remaining %4 s (scale %5)", drugName, baseRate, effectiveRate, remaining, bloodFlowScale));
			}
		}
		
		DbgUI.End();
	}
#endif
}
