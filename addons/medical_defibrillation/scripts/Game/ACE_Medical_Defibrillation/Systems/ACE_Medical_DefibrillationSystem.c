/*
	Will need to extend/modify this system if manual defibs are added. Will need a manual defib component.
*/

class ACE_Medical_DefibrillationSystem : ACE_Medical_BaseSystem3
{
	protected ACE_Medical_DefibrillationSystemSettings m_Settings;
	
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
		
		Print("ACE_Medical_DefibrillationSystem.OnInit:: Defib system starting...");
		super.OnInit();
	}

	//------------------------------------------------------------------------------------------------
	protected override void Update(IEntity entity, float timeSlice)
	{
		super.Update(entity, timeSlice);
		
		ACE_Medical_AEDComponent component = GetAEDComponent(entity);
		if (!component)
			return;
		
		UpdateAED(entity, timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	override void Register(notnull IEntity entity)
	{
		super.Register(entity);
		
		PrintFormat("ACE_Medical_DefibrillationSystem.Register:: Entity <%1> Registered", entity.ClassName());
	}
		
	//------------------------------------------------------------------------------------------------
	protected void UpdateAED(IEntity entity, float timeSlice)
	{
		ACE_Medical_AEDComponent component = GetAEDComponent(entity);
		if (!component)
			return;
		
		UpdateCharge(entity, timeSlice);
		UpdateAnalysis(entity, timeSlice);
		UpdateSoundEffects(entity, timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateCharge(IEntity entity, float timeSlice)
	{
		ACE_Medical_AEDComponent component = GetAEDComponent(entity);
		if (!component)
			return;
		
		float charge = component.GetChargeAmount();
		float chargeTime = component.GetChargeTime();
		
		if (component.IsCharging())
		{
			PrintFormat("ACE_Medical_DefibrillationSystem.UpdateAnalysis:: Amount to add to charge = %1", timeSlice / chargeTime);
			charge += timeSlice / chargeTime;
			component.SetChargeAmount(charge);
			
			PrintFormat("ACE_Medical_DefibrillationSystem.UpdateAnalysis:: Current charge percent = %1", component.GetChargeAmount());
			
			if (charge >= 1.0)
			{
				component.SetChargeAmount(1.0);
				component.SetIsCharged(true);
				component.SetCharging(false);
				component.TerminateSound();
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateAnalysis(IEntity entity, float timeSlice)
	{
		ACE_Medical_AEDComponent component = GetAEDComponent(entity);
		if (!component)
			return;
		
		float analysis = component.GetAnalysisAmount();
		float analysisTime = component.GetAnalysisTime();
		
		if (component.IsAnalysing())
		{
			PrintFormat("ACE_Medical_DefibrillationSystem.UpdateAnalysis:: Amount to add to analysis = %1", timeSlice / analysisTime);
			analysis += timeSlice / analysisTime;
			component.SetAnalysisAmount(analysis);

			PrintFormat("ACE_Medical_DefibrillationSystem.UpdateAnalysis:: Current analysis percent = %1", component.GetAnalysisAmount());
			
			if (analysis >= 1.0)
			{
				component.SetAnalysisAmount(1.0);
				component.SetIsAnalysed(true);
				component.SetAnalysing(false);
				// Testing - need to impliment rhythm check
				if (component.IsShockableRhythm())
				{
					component.PlaySound(ACE_Medical_AEDComponent.SOUNDSHOCKADVISED, true);
				}
				else
				{
					component.PlaySound(ACE_Medical_AEDComponent.SOUNDNOSHOCKADVISED, true);
					component.ResetAnalysisAndCharge();
				}
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateSoundEffects(IEntity entity, float timeSlice)
	{
		ACE_Medical_AEDComponent component = GetAEDComponent(entity);
		if (!component)
			return;
		
		if (component.GetChargeAmount() >= 1.0 && !component.IsCharging())
		{
			component.PlaySound(ACE_Medical_AEDComponent.SOUNDCHARGED, isLoop: true);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	static ACE_Medical_AEDComponent GetAEDComponent(IEntity entity)
	{
		return ACE_Medical_AEDComponent.Cast(entity.FindComponent(ACE_Medical_AEDComponent));
	}
	
#ifdef WORKBENCH
	//------------------------------------------------------------------------------------------------
	override protected void OnDiag(float timeSlice)
	{
		super.OnDiag(timeSlice);
		
		DbgUI.Begin("ACE_Medical_DefibrillationSystem");
		
		DbgUI.Text("AEDs: " + m_aQueue.Count());
		
		DbgUI.End();
	}
#endif
}