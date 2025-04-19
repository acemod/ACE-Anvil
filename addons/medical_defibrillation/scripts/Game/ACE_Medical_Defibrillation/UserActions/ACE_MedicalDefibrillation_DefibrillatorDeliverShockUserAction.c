class ACE_MedicalDefibrillation_DefibrillatorDeliverShockUserAction : ACE_MedicalDefibrillator_DefibrillatorBaseUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(GetOwner().FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return false;
		
		if (!defibrillatorComponent.IsReadyToShock())
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
		
		PrintFormat("ACE_MedicalDefibrillation_DefibrillatorDeliverShockUserAction::Perform Action | Server Execution: %1", Replication.IsServer());
		
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(GetOwner().FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return;
		
		if (!defibrillatorComponent.IsReadyToShock())
			return;
		
		// shock and notify
		if(defibrillatorComponent.DeliverShock())
		{
			ACE_MedicalDefibrillation_NetworkComponent networkComponent;
			if (!GetDefibrillatorNetworkComponent(pUserEntity, networkComponent))
				return;
			
			networkComponent.RequestDefibrillatorNotification(ENotification.ACE_MEDICALDEFIBRILLATION_SHOCKDELIVERED, GetOwner(), SCR_ChimeraCharacter.Cast(defibrillatorComponent.GetConnectedPatient()));
		}
	}
}