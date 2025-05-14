//------------------------------------------------------------------------------------------------
class ACE_MedicalDefibrillation_DefibrillatorPickUpItemAction : SCR_PickUpItemAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
			return;
		
		ACE_MedicalDefibrillation_DefibrillatorComponent defibrillatorComponent = ACE_MedicalDefibrillation_DefibrillatorComponent.Cast(GetOwner().FindComponent(ACE_MedicalDefibrillation_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return;
		
		// Reset defibrillator
		defibrillatorComponent.ResetPatient();
		
		// Unsubscribe from system
		defibrillatorComponent.UnregisterFromSystem(GetOwner());
		
		super.PerformAction(pOwnerEntity, pUserEntity);
	}
}