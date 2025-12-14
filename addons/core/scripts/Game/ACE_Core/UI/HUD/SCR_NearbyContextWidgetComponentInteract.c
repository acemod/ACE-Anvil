//------------------------------------------------------------------------------------------------
//! Colorize SCR_HealingUserAction
/*
modded class SCR_NearbyContextWidgetComponentInteract : SCR_ScriptedWidgetComponent
{
	//------------------------------------------------------------------------------------------------
	override protected bool IsHealingAction(BaseUserAction userAct, out bool showOuterBackground, out int nearbyColorState)
	{
		if (super.IsHealingAction(userAct, showOuterBackground, nearbyColorState))
			return true;
		
		SCR_HealingUserAction healingUserAction = SCR_HealingUserAction.Cast(userAct);
		if (!healingUserAction)
			return false;
		
		nearbyColorState = healingUserAction.ACE_GetHealthStatus();
		showOuterBackground = true;
		return true;
	}
}
