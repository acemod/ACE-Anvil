class ACE_Medical_DefibrillatorDeliverShockUserAction : ACE_Medical_DefibrillatorBaseUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		ACE_Medical_DefibrillatorComponent defibrillatorComponent = ACE_Medical_DefibrillatorComponent.Cast(GetOwner().FindComponent(ACE_Medical_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return false;
		
		if (!defibrillatorComponent.IsReadyToShock())
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
		
		if (!defibrillatorComponent.IsReadyToShock())
			return;
		
		defibrillatorComponent.DeliverShock();
		
		ACE_Medical_DefibrillatorNetworkComponent networkComponent = ACE_Medical_DefibrillatorNetworkComponent.Cast(pOwnerEntity.FindComponent(ACE_Medical_DefibrillatorNetworkComponent));
		if (!networkComponent)
			return;
		
		//networkComponent.RequestDefibrillatorNotification(ENotification.ACE_MEDICAL_SHOCKDELIVERED, GetOwner(), SCR_ChimeraCharacter.Cast(defibrillatorComponent.GetConnectedPatient()));
	}
}