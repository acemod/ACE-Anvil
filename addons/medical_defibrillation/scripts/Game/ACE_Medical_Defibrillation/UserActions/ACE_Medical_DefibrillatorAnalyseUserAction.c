class ACE_Medical_DefibrillatorAnalyseUserAction : ACE_Medical_DefibrillatorBaseUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		ACE_Medical_DefibrillatorComponent defibrillatorComponent = ACE_Medical_DefibrillatorComponent.Cast(GetOwner().FindComponent(ACE_Medical_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return false; 
		
		if (defibrillatorComponent.GetDefibrillationEmulation() == ACE_Medical_EDefibrillatorEmulation.Manual)
			return false;
		
		if (!defibrillatorComponent.GetConnectedPatient())
			return false;
		
		if (defibrillatorComponent.IsAnalysing())
			return false;
		
		if (defibrillatorComponent.IsCharging())
			return false;
		
		if (defibrillatorComponent.IsAnalysed() && defibrillatorComponent.IsShockableRhythm())
			return false;
		
		if (defibrillatorComponent.IsCharged())
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		ACE_Medical_DefibrillatorComponent defibrillatorComponent = ACE_Medical_DefibrillatorComponent.Cast(GetOwner().FindComponent(ACE_Medical_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return false; 
		
		ACE_Medical_CardiovascularComponent cardiovascularComponent;
		if (!defibrillatorComponent.GetConnectedPatientCardiovascularComponent(cardiovascularComponent))
			return false;
		
		if (cardiovascularComponent.GetShockCooldown() > 0)
		{
			SetCannotPerformReason(string.Format("Analyse Disabled - Continue CPR (%1 s)", cardiovascularComponent.GetShockCooldown()));
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.PerformAction(pOwnerEntity, pUserEntity);
		
		ACE_Medical_DefibrillatorComponent defibrillatorComponent = ACE_Medical_DefibrillatorComponent.Cast(GetOwner().FindComponent(ACE_Medical_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return;
		
		if (!defibrillatorComponent.GetConnectedPatient())
			return;
		
		defibrillatorComponent.AnalyzeRhythm();
	}
}