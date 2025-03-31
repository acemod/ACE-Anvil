class ACE_Medical_AEDChargeUserAction : ACE_Medical_AEDBaseUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		ACE_Medical_AEDComponent AEDComp = ACE_Medical_AEDComponent.Cast(this.GetOwner().FindComponent(ACE_Medical_AEDComponent));
		if (!AEDComp)
			return false;
		
		if (!AEDComp.PatientConnected())
			return false;
		
		if (AEDComp.GetAnalysis() < 1.0)
			return false;
		
		if (AEDComp.GetCharge() >= 1.0)
			return false;
		
		if (AEDComp.IsCharging())
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		ACE_Medical_AEDComponent AEDComp = ACE_Medical_AEDComponent.Cast(this.GetOwner().FindComponent(ACE_Medical_AEDComponent));
		if (!AEDComp)
			return;
		
		if (!AEDComp.PatientConnected())
			return;
		
		AEDComp.Charge();
	}
}