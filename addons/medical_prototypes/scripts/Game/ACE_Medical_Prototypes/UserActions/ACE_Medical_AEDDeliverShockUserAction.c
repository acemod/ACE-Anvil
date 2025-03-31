class ACE_Medical_AEDDeliverShockUserAction : ACE_Medical_AEDBaseUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		ACE_Medical_AEDComponent AEDComp = ACE_Medical_AEDComponent.Cast(this.GetOwner().FindComponent(ACE_Medical_AEDComponent));
		if (!AEDComp)
			return false;
		
		if (!AEDComp.IsReadyToShock())
			return false;
		
		return true;
	}
}