//------------------------------------------------------------------------------------------------
class ACE_Medical_DefibrillatorPickUpItemAction : SCR_PickUpItemAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.PerformAction(pOwnerEntity, pUserEntity);
		
		ACE_Medical_DefibrillatorComponent defibrillatorComponent = ACE_Medical_DefibrillatorComponent.Cast(GetOwner().FindComponent(ACE_Medical_DefibrillatorComponent));
		if (!defibrillatorComponent)
			return;
		
		// Reset defibrillator
		defibrillatorComponent.ResetPatient();
		
		// Unsubscribe from system
		defibrillatorComponent.UnregisterFromSystem(GetOwner());
	}
}