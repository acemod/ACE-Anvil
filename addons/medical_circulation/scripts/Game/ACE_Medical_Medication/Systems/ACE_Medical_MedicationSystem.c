//------------------------------------------------------------------------------------------------
#ifdef ENABLE_DIAG
class ACE_Medical_Medication_DiagInfusionState
{
	float m_fPreviousElapsed;
	float m_fPreviousEffectiveRate;
	float m_fDeliveredVolume;
}

class ACE_Medical_Medication_DiagState
{
	float m_fPreviousBloodVolume;
	float m_fSmoothedNetRate;
	float m_fSmoothedBleedRate;
	float m_fSmoothedPassiveRate;
	bool m_bHasPreviousVolume;
	bool m_bHasRates;
	ref map<ACE_Medical_Infusion, ref ACE_Medical_Medication_DiagInfusionState> m_mInfusionStates;
	float m_fCumulativeSalineIntake;
	
	void ACE_Medical_Medication_DiagState()
	{
		m_mInfusionStates = new map<ACE_Medical_Infusion, ref ACE_Medical_Medication_DiagInfusionState>();
	}
	
	void UpdatePrevious(float volume)
	{
		m_fPreviousBloodVolume = volume;
		m_bHasPreviousVolume = true;
	}
	
	void UpdateRates(float netRate, float bleedRate, float passiveRate, float alpha)
	{
		if (!m_bHasRates)
		{
			m_fSmoothedNetRate = netRate;
			m_fSmoothedBleedRate = bleedRate;
			m_fSmoothedPassiveRate = passiveRate;
			m_bHasRates = true;
			return;
		}
		
		m_fSmoothedNetRate = m_fSmoothedNetRate + alpha * (netRate - m_fSmoothedNetRate);
		m_fSmoothedBleedRate = m_fSmoothedBleedRate + alpha * (bleedRate - m_fSmoothedBleedRate);
		m_fSmoothedPassiveRate = m_fSmoothedPassiveRate + alpha * (passiveRate - m_fSmoothedPassiveRate);
	}
	
	void ClearMissingInfusions(array<ACE_Medical_Infusion> activeInfusions)
	{
		if (!m_mInfusionStates)
			return;
		
		array<ACE_Medical_Infusion> removals();
		foreach (ACE_Medical_Infusion trackedInfusion, ACE_Medical_Medication_DiagInfusionState state : m_mInfusionStates)
		{
			if (activeInfusions.Find(trackedInfusion) < 0)
				removals.Insert(trackedInfusion);
		}
		
		foreach (ACE_Medical_Infusion infusionToRemove : removals)
		{
			m_mInfusionStates.Remove(infusionToRemove);
		}
	}
}
#endif

//------------------------------------------------------------------------------------------------
class ACE_Medical_MedicationSystem : GameSystem
{
	[Attribute()]
	protected ref ACE_Medical_Medication_JobScheduler m_pScheduler;
	
#ifdef ENABLE_DIAG
	protected static ref ACE_Medical_Medication_DiagState s_pDiagState = new ACE_Medical_Medication_DiagState();
	protected static IEntity s_pDiagTarget;
	
	//------------------------------------------------------------------------------------------------
	protected static string FormatRateForDisplay(float value)
	{
		if (Math.AbsFloat(value) < 0.005)
			return "0.00";
		
		float rounded = Math.Round(value * 100.0) / 100.0;
		float absRounded = Math.AbsFloat(rounded);
		int whole = Math.Floor(absRounded);
		int decimals = Math.Round((absRounded - whole) * 100.0);
		
		if (decimals >= 100)
		{
			whole += 1;
			decimals -= 100;
		}
		
		string wholeText = string.Format("%1", whole);
		string decimalText = string.Format("%1", decimals);
		if (decimals < 10)
			decimalText = string.Format("0%1", decimalText);
		
		string sign = "";
		if (rounded < 0)
			sign = "-";
		
		return string.Format("%1%2.%3", sign, wholeText, decimalText);
	}

#endif

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
		
		ACE_Medical_Medication_DiagState diagState = s_pDiagState;
		if (!diagState || target != s_pDiagTarget)
		{
			diagState = new ACE_Medical_Medication_DiagState();
			s_pDiagState = diagState;
			s_pDiagTarget = target;
		}
		
		ACE_Medical_CharacterContext targetContext = new ACE_Medical_CharacterContext(SCR_ChimeraCharacter.Cast(target));
		ACE_Medical_Medication_SimulationJob job = CreateTemplateJob();
		job.SetContext(targetContext);
		ACE_Medical_Medication_Settings settings = ACE_SettingsHelperT<ACE_Medical_Medication_Settings>.GetModSettings();
		
		DbgUI.Begin(string.Format("ACE_Medical_MedicationSystem (%1)", targetType), 700);
		DbgUI.Text("Drugs:\n===============================");
		array<ACE_Medical_EDrugType> drugs;
		array<ref array<ref ACE_Medical_Dose>> doses;
		targetContext.m_pMedication.GetMedications(drugs, doses);
		
		float bloodFlowScale = 1;
		if (targetContext.m_pDamageManager)
			bloodFlowScale = targetContext.m_pDamageManager.ACE_Medical_GetBloodFlowScale();
		
		SCR_CharacterBloodHitZone bloodHitZone = targetContext.m_pBloodHitZone;
		float bloodVolume = 0;
		float bleedRate = 0;
		if (bloodHitZone)
		{
			bloodVolume = bloodHitZone.GetHealth();
			bleedRate = bloodHitZone.GetTotalBleedingAmount();
		}
		
		ACE_Medical_PharmacokineticsConfig salineKinetics;
		float salineEliminationRate = 0;
		if (settings)
		{
			salineKinetics = settings.GetPharmacokineticsConfig(ACE_Medical_EDrugType.SALINE);
			if (salineKinetics)
				salineEliminationRate = salineKinetics.m_fDeactivationRateConstant;
		}
		
		map<ACE_Medical_EDrugType, float> concentrations = job.ComputeConcentrations(drugs, doses, deleteExpiredDoses: false);
		
		bool hasDrugConcentrations = false;
		bool hasSalineConcentration = false;
		float salineReservoir = 0;
		float salineThroughput = 0;
		
		foreach (ACE_Medical_EDrugType drug, float concentration : concentrations)
		{
			string drugName = SCR_Enum.GetEnumName(ACE_Medical_EDrugType, drug);
			if (drug == ACE_Medical_EDrugType.SALINE)
			{
				hasSalineConcentration = true;
				salineReservoir = concentration;
				if (salineEliminationRate > 0)
					salineThroughput = concentration * salineEliminationRate;
				continue;
			}
			
			hasDrugConcentrations = true;
			DbgUI.Text(string.Format("- %1:\t%2 nM", drugName, concentration));
			DbgUI.PlotLive(drugName, 500, 100, concentration, 100, 100000);
		}
		
		if (!hasDrugConcentrations)
			DbgUI.Text("(no active drug concentrations)");
		
		if (hasSalineConcentration)
		{
			DbgUI.Text("\nFluids:\n===============================");
			if (salineEliminationRate > 0)
				DbgUI.Text(string.Format("- Throughput (ml/s):\t%1", FormatRateForDisplay(salineThroughput)));
			DbgUI.PlotLive("SALINE", 500, 100, salineReservoir, 100, 100000);
		}
		
		if (settings)
		{
			DbgUI.Text("\nEffects:\n===============================");
			
			foreach (ACE_Medical_DrugEffectConfig effect : settings.m_aPharmacodynamicsConfigs)
			{
				effect.OnDiag(targetContext, timeSlice);
			}
		}
		
		array<string> drugInfusionLines();
		array<string> salineInfusionLines();
		array<ACE_Medical_Infusion> activeSalineInfusions();
		float totalEffectiveInfusionRate = 0;
		float totalSalineBagCapacity = 0;
		
		for (int doseIdx = 0; doseIdx < doses.Count(); doseIdx++)
		{
			array<ref ACE_Medical_Dose> drugDoses = doses[doseIdx];
			foreach (ACE_Medical_Dose dose : drugDoses)
			{
				ACE_Medical_Infusion infusion = ACE_Medical_Infusion.Cast(dose);
				if (!infusion)
					continue;

				float baseRate = infusion.GetInfusionRate();
				float effectiveRate = baseRate * bloodFlowScale;
				float elapsed = infusion.GetElapsedTime();
				float duration = infusion.GetInfusionDuration();
				float remaining = Math.Max(0, duration - elapsed);
				ACE_Medical_EDrugType drugType = drugs[doseIdx];
				string drugName = SCR_Enum.GetEnumName(ACE_Medical_EDrugType, drugType);
				string rateUnits = "nM/s";
				if (drugType == ACE_Medical_EDrugType.SALINE)
					rateUnits = "ml/s";
				string infusionLine = string.Format("- %1: base %2 %6, effective %3 %6, remaining %4 s (scale %5)", drugName, baseRate, effectiveRate, remaining, bloodFlowScale, rateUnits);
				if (drugType == ACE_Medical_EDrugType.SALINE)
				{
					salineInfusionLines.Insert(infusionLine);
					activeSalineInfusions.Insert(infusion);
					totalEffectiveInfusionRate += effectiveRate;
					float activeTime = Math.Min(elapsed, duration);
					totalSalineBagCapacity += baseRate * duration;
					ACE_Medical_Medication_DiagInfusionState infusionState;
					if (!diagState.m_mInfusionStates.Find(infusion, infusionState))
					{
						infusionState = new ACE_Medical_Medication_DiagInfusionState();
						diagState.m_mInfusionStates.Set(infusion, infusionState);
					}
					
					float deltaElapsed = Math.Max(0, elapsed - infusionState.m_fPreviousElapsed);
					float averagedEffective = effectiveRate;
					if (infusionState.m_fPreviousElapsed > 0 && deltaElapsed > 0)
						averagedEffective = 0.5 * (infusionState.m_fPreviousEffectiveRate + effectiveRate);
					infusionState.m_fDeliveredVolume += averagedEffective * deltaElapsed;
					infusionState.m_fPreviousElapsed = elapsed;
					infusionState.m_fPreviousEffectiveRate = effectiveRate;
				}
				else
				{
					drugInfusionLines.Insert(infusionLine);
				}
			}
		}
		
		if (hasSalineConcentration && salineEliminationRate > 0 && timeSlice > 0)
			diagState.m_fCumulativeSalineIntake += salineThroughput * timeSlice;
		
		if (diagState.m_mInfusionStates)
			diagState.ClearMissingInfusions(activeSalineInfusions);
		
		float totalSalineBagDelivered = 0;
		if (diagState.m_mInfusionStates)
		{
			foreach (ACE_Medical_Infusion trackedInfusion, ACE_Medical_Medication_DiagInfusionState trackedState : diagState.m_mInfusionStates)
				totalSalineBagDelivered += trackedState.m_fDeliveredVolume;
		}
		
		if (drugInfusionLines.Count() > 0)
		{
			DbgUI.Text("\nInfusions:\n===============================");
			foreach (string line : drugInfusionLines)
				DbgUI.Text(line);
		}
		
		if (salineInfusionLines.Count() > 0)
		{
			DbgUI.Text("\nFluid infusions:\n===============================");
			foreach (string line : salineInfusionLines)
				DbgUI.Text(line);
			if (salineInfusionLines.Count() > 1)
			{
				string totalEffectiveDisplay = FormatRateForDisplay(totalEffectiveInfusionRate);
				DbgUI.Text(string.Format("- Total saline effective rate: %1 ml/s", totalEffectiveDisplay));
			}
			float remainingMl = Math.Max(0, totalSalineBagCapacity - totalSalineBagDelivered);
			DbgUI.Text(string.Format("- Bag delivered: %1 / %2 ml (remaining %3 ml)",
				FormatRateForDisplay(totalSalineBagDelivered),
				FormatRateForDisplay(totalSalineBagCapacity),
				FormatRateForDisplay(remainingMl)));
			DbgUI.Text(string.Format("- Patient uptake: %1 ml (rate %2 ml/s)",
				FormatRateForDisplay(diagState.m_fCumulativeSalineIntake),
				FormatRateForDisplay(salineThroughput)));
		}
		
		string passiveDisplay = "collecting...";
		string bleedDisplay = "collecting...";
		string netDisplay = "collecting...";
		if (bloodHitZone)
		{
			bleedDisplay = FormatRateForDisplay(bleedRate);
			if (diagState)
			{
				if (diagState.m_bHasPreviousVolume && timeSlice > 0)
				{
					float netRate = (bloodVolume - diagState.m_fPreviousBloodVolume) / timeSlice;
					float passiveRate = netRate + bleedRate - totalEffectiveInfusionRate;
					float smoothingWindow = 1.0;
					float alpha = timeSlice / smoothingWindow;
					if (alpha > 1.0)
						alpha = 1.0;
					diagState.UpdateRates(netRate, bleedRate, passiveRate, alpha);
					passiveDisplay = FormatRateForDisplay(diagState.m_fSmoothedPassiveRate);
					bleedDisplay = FormatRateForDisplay(diagState.m_fSmoothedBleedRate);
					netDisplay = FormatRateForDisplay(diagState.m_fSmoothedNetRate);
				}
				diagState.UpdatePrevious(bloodVolume);
			}
		}
		DbgUI.Text(string.Format("\nVolume recon (~1s, ml/s): passive %1 | bleeding %2 | net %3", passiveDisplay, bleedDisplay, netDisplay));
		
		DbgUI.End();
	}
#endif
}

