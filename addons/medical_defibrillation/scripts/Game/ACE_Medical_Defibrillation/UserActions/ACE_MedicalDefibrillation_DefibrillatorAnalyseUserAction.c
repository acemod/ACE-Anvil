class ACE_MedicalDefibrillation_DefibrillatorAnalyseUserAction : ACE_MedicalDefibrillator_DefibrillatorBaseUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(GetOwner().FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return false; 
		
		if (defibrillatorComponent.GetDefibrillationEmulation() == ACE_MedicalDefibrillation_EDefibrillatorEmulation.Manual_NotImplimented)
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
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(GetOwner().FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
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
		if (!Replication.IsServer())
			return;
		
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(GetOwner().FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return;
		
		if (!defibrillatorComponent.GetConnectedPatient())
			return;
		
		defibrillatorComponent.AnalyzeRhythm();
	}
}