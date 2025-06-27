class ACE_MedicalDefibrillation_DefibrillatorDeliverShockUserAction : ACE_MedicalDefibrillator_DefibrillatorBaseUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		super.CanBeShownScript(user);
		
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(GetOwner().FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return false;
		
		if (defibrillatorComponent.GetStateEnum() != ACE_MedicalDefibrillation_EDefibrillatorState.Charged)
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
		
		if (defibrillatorComponent.GetStateEnum() != ACE_MedicalDefibrillation_EDefibrillatorState.Charged)
			return;
		
		if(defibrillatorComponent.DeliverShock())
		{
			ACE_Medical_NetworkComponent networkComponent = ACE_Medical_NetworkComponent.GetACEMedicalNetworkComponent(pUserEntity);
			if (!networkComponent)	
				return;
			
			networkComponent.RequestDefibrillatorNotification(ENotification.ACE_MEDICALDEFIBRILLATION_SHOCKDELIVERED, GetOwner(), SCR_ChimeraCharacter.Cast(defibrillatorComponent.GetConnectedPatient()));
		}
	}
}