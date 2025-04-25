class ACE_MedicalDefibrillation_DefibrillatorDisconnectPatientUserAction : ACE_MedicalDefibrillator_DefibrillatorBaseUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(GetOwner().FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return false;
		
		if (!defibrillatorComponent.GetConnectedPatient())
			return false;
		
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
		
		defibrillatorComponent.ResetPatient();
		
		ACE_MedicalDefibrillation_NetworkComponent networkComponent;
		if (!ACE_MedicalDefibrillator_DefibrillatorBaseUserAction.GetDefibrillatorNetworkComponent(pUserEntity, networkComponent))
			return;
		
		networkComponent.RequestDefibrillatorNotification(ENotification.ACE_MEDICALDEFIBRILLATION_PATIENTDISCONNECTED, defibrillatorComponent.GetOwner());
	}
}