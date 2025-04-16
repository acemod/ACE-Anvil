class ACE_Medical_DefibrillatorDisconnectPatientUserAction : ACE_Medical_DefibrillatorBaseUserAction
{
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		ACE_Medical_DefibrillatorComponent defibrillatorComponent = ACE_Medical_DefibrillatorComponent.Cast(GetOwner().FindComponent(ACE_Medical_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return false;
		
		if (!defibrillatorComponent.GetConnectedPatient())
			return false;
		
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
		
		defibrillatorComponent.ResetPatient();
	}
}