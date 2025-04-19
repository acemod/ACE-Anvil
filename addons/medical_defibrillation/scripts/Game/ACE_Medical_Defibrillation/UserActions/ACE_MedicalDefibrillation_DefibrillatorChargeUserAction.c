class ACE_MedicalDefibrillation_DefibrillatorChargeUserAction : ACE_MedicalDefibrillator_DefibrillatorBaseUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(GetOwner().FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return false;
		
		if (!defibrillatorComponent.GetConnectedPatient())
			return false;
		
		if (!defibrillatorComponent.IsAnalysed())
			return false;
		
		if (defibrillatorComponent.IsCharged())
			return false;
		
		if (defibrillatorComponent.IsCharging())
			return false;
		
		// Guardrails for automated defibrillation
		if (defibrillatorComponent.GetDefibrillationEmulation() == ACE_MedicalDefibrillation_EDefibrillatorEmulation.Automated)
		{
			if (!defibrillatorComponent.IsShockableRhythm())
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
		
		defibrillatorComponent.Charge();
	}
}