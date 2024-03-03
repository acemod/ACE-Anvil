//------------------------------------------------------------------------------------------------
modded class SCR_GetInUserAction : SCR_CompartmentUserAction
{
	//------------------------------------------------------------------------------------------------
	//! Disable get in user action if player is currently carrying another
	override bool CanBePerformedScript(IEntity user)
	{
		if (CEN_CarrierSystem_Helper.IsCarrier(user))
		{
			SetCannotPerformReason("#CEN_CarrierSystem-UserAction_Carrying");
			return false;
		};
		
		return super.CanBePerformedScript(user);
	}	
};
