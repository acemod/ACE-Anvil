//------------------------------------------------------------------------------------------------
class ACE_Medical_AEDPickUpItemAction : SCR_PickUpItemAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.PerformAction(pOwnerEntity, pUserEntity);
		
		ACE_Medical_AEDComponent AEDComp = ACE_Medical_AEDComponent.Cast(this.GetOwner().FindComponent(ACE_Medical_AEDComponent));
		if (!AEDComp)
			return;
		
		// Reset AED
		AEDComp.ResetPatient();
		
		// Unsubscribe from system
		AEDComp.UnregisterFromSystem(this.GetOwner());
	}
}