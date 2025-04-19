/*
	Will need to extend/modify this system if manual defibs are added. Will need a manual defib component.
*/

class ACE_MedicalDefibrillation_DefibrillationSystem : ACE_Medical_BaseSystem3
{
	protected ACE_MedicalDefibrillation_DefibrillationSystemSettings m_Settings;
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(false)
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (settings && settings.m_DefibrillationSystem)
			m_Settings = settings.m_DefibrillationSystem;
		
		Print("ACE_MedicalDefibrillation_DefibrillationSystem.OnInit:: Defib system starting...");
		super.OnInit();
	}

	//------------------------------------------------------------------------------------------------
	protected override void Update(IEntity entity, float timeSlice)
	{
		super.Update(entity, timeSlice);
		
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(entity.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return;
		
		if (defibrillatorComponent.GetDefibrillationEmulation() == ACE_MedicalDefibrillation_EDefibrillatorEmulation.Automated)
			UpdateAED(entity, timeSlice);
		
		// TODO: Impliment manual defibrillator
		if (defibrillatorComponent.GetDefibrillationEmulation() == ACE_MedicalDefibrillation_EDefibrillatorEmulation.Manual)
		{
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void Register(notnull IEntity entity)
	{
		super.Register(entity);
		
		PrintFormat("ACE_MedicalDefibrillation_DefibrillationSystem.Register:: Entity <%1> Registered", entity.ClassName());
	}
		
	//------------------------------------------------------------------------------------------------
	protected void UpdateAED(IEntity entity, float timeSlice)
	{
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(entity.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return;
		
		UpdateCharge(entity, timeSlice);
		UpdateAnalysis(entity, timeSlice);
		UpdateSoundEffects(entity, timeSlice);
		CheckPatientPosition(entity, timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateCharge(IEntity entity, float timeSlice)
	{
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(entity.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return;
		
		float charge = defibrillatorComponent.GetChargeAmount();
		float chargeTime = defibrillatorComponent.GetChargeTime();
		
		if (defibrillatorComponent.IsCharging())
		{
			PrintFormat("ACE_MedicalDefibrillation_DefibrillationSystem.UpdateAnalysis:: Amount to add to charge = %1", timeSlice / chargeTime);
			charge += timeSlice / chargeTime;
			defibrillatorComponent.SetChargeAmount(charge);
			
			PrintFormat("ACE_MedicalDefibrillation_DefibrillationSystem.UpdateAnalysis:: Current charge percent = %1", defibrillatorComponent.GetChargeAmount());
			
			if (charge >= 1.0)
			{
				defibrillatorComponent.SetChargeAmount(1.0);
				defibrillatorComponent.SetIsCharged(true);
				defibrillatorComponent.SetCharging(false);
				defibrillatorComponent.TerminateSound();
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateAnalysis(IEntity entity, float timeSlice)
	{
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(entity.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return;
		
		float analysis = defibrillatorComponent.GetAnalysisAmount();
		float analysisTime = defibrillatorComponent.GetAnalysisTime();
		
		if (defibrillatorComponent.IsAnalysing())
		{
			PrintFormat("ACE_MedicalDefibrillation_DefibrillationSystem.UpdateAnalysis:: Amount to add to analysis = %1", timeSlice / analysisTime);
			analysis += timeSlice / analysisTime;
			defibrillatorComponent.SetAnalysisAmount(analysis);

			PrintFormat("ACE_MedicalDefibrillation_DefibrillationSystem.UpdateAnalysis:: Current analysis percent = %1", defibrillatorComponent.GetAnalysisAmount());
			
			if (analysis >= 1.0)
			{
				defibrillatorComponent.SetAnalysisAmount(1.0);
				defibrillatorComponent.SetIsAnalysed(true);
				defibrillatorComponent.SetAnalysing(false);
				// Testing - need to impliment rhythm check
				if (defibrillatorComponent.IsShockableRhythm())
				{
					defibrillatorComponent.PlaySound(defibrillatorComponent.SOUNDSHOCKADVISED, true);
				}
				else
				{
					defibrillatorComponent.PlaySound(defibrillatorComponent.SOUNDNOSHOCKADVISED, true);
					
					defibrillatorComponent.ResetAnalysisAndCharge();
				}
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateSoundEffects(IEntity entity, float timeSlice)
	{
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(entity.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return;
		
		if (defibrillatorComponent.GetChargeAmount() >= 1.0 && !defibrillatorComponent.IsCharging())
		{
			defibrillatorComponent.PlaySound(defibrillatorComponent.SOUNDCHARGED, true, true);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CheckPatientPosition(IEntity entity, float timeSlice)
	{
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(entity.FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return;
		
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(defibrillatorComponent.GetConnectedPatient());
		if (!char)
			return;
		
		vector charPos = char.GetOrigin();
		vector AEDPos = entity.GetOrigin();
		
		float distance = vector.Distance(charPos, AEDPos);
		if (distance >= 3)
			defibrillatorComponent.ResetPatient();
	}

#ifdef WORKBENCH
	//------------------------------------------------------------------------------------------------
	override protected void OnDiag(float timeSlice)
	{
		super.OnDiag(timeSlice);
		
		DbgUI.Begin("ACE_MedicalDefibrillation_DefibrillationSystem");
		
		DbgUI.Text("AEDs: " + m_aQueue.Count());
		
		DbgUI.End();
	}
#endif
}