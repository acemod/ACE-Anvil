class ACE_Medical_DefibrillatorChargeUserAction : ACE_Medical_DefibrillatorBaseUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		ACE_Medical_DefibrillatorComponent defibrillatorComponent = ACE_Medical_DefibrillatorComponent.Cast(GetOwner().FindComponent(ACE_Medical_DefibrillatorComponent));
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
		if (defibrillatorComponent.GetDefibrillationEmulation() == ACE_Medical_EDefibrillatorEmulation.Automated)
		{
			if (!defibrillatorComponent.IsShockableRhythm())
				return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		ACE_Medical_DefibrillatorComponent defibrillatorComponent = ACE_Medical_DefibrillatorComponent.Cast(GetOwner().FindComponent(ACE_Medical_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return;
		
		if (!defibrillatorComponent.GetConnectedPatient())
			return;
		
		defibrillatorComponent.Charge();
	}
}