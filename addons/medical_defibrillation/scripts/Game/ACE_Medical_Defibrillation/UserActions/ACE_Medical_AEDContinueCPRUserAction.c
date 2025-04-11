class ACE_Medical_AEDContinueCPRUserAction : ACE_Medical_AEDBaseUserAction
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
		
		if (AEDComp.IsAnalysed())
			return false;
		
		if (AEDComp.IsCharged())
			return false;
		
		ACE_Medical_CardiovascularComponent component = AEDComp.GetConnectedPatientCardiovascularComponent();
		if (!component)
			return false;
		
		UIInfo uiInfo = GetUIInfo();
		if (component.GetShockCooldown() > 0)
		{
			uiInfo.SetName(string.Format("Analyse Disabled - Continue CPR (%1 s)", component.GetShockCooldown()));
			return true;
		}
		else
		{
			uiInfo.SetName(string.Format("Analyse Disabled - Continue CPR"));
		}
		
		return false;
	}
}