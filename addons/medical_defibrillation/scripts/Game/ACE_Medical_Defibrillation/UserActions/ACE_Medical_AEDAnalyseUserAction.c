class ACE_Medical_AEDAnalyseUserAction : ACE_Medical_AEDBaseUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		ACE_Medical_AEDComponent AEDComp = ACE_Medical_AEDComponent.Cast(this.GetOwner().FindComponent(ACE_Medical_AEDComponent));
		if (!AEDComp)
			return false; 
		
		if (!AEDComp.GetConnectedPatient())
			return false;
		
		if (AEDComp.IsAnalysing())
			return false;
		
		if (AEDComp.IsCharging())
			return false;
		
		if (AEDComp.IsAnalysed() && AEDComp.IsShockableRhythm())
			return false;
		
		if (AEDComp.IsCharged())
			return false;
		
		ACE_Medical_CardiovascularComponent component = AEDComp.GetConnectedPatientCardiovascularComponent();
		if (component)
		{
			if (component.GetShockCooldown() > 0)
				return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		ACE_Medical_AEDComponent AEDComp = ACE_Medical_AEDComponent.Cast(this.GetOwner().FindComponent(ACE_Medical_AEDComponent));
		if (!AEDComp)
			return;
		
		if (!AEDComp.GetConnectedPatient())
			return;
		
		AEDComp.AnalyzeRhythm();
	}
}