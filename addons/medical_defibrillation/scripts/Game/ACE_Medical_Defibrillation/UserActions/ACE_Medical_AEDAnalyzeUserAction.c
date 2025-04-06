class ACE_Medical_AEDAnalyzeUserAction : ACE_Medical_AEDBaseUserAction
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
		
		if (AEDComp.GetAnalysisAmount() >= 1.0)
			return false;
		
		if (AEDComp.GetChargeAmount() >= 1.0)
			return false;
		
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