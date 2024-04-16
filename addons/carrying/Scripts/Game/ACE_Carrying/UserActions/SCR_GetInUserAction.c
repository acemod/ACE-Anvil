//------------------------------------------------------------------------------------------------
modded class SCR_GetInUserAction : SCR_CompartmentUserAction
{
	//------------------------------------------------------------------------------------------------
	//! Disable get in user action if player is currently carrying another
	override bool CanBePerformedScript(IEntity user)
	{
		if (ACE_Carrying_Tools.IsCarrier(user))
		{
			SetCannotPerformReason("#ACE_Carrying-UserAction_Carrying");
			return false;
		};
		
		return super.CanBePerformedScript(user);
	}	
};
