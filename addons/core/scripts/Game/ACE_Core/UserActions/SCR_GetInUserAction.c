//------------------------------------------------------------------------------------------------
modded class SCR_GetInUserAction : SCR_CompartmentUserAction
{
	//------------------------------------------------------------------------------------------------
	//! Disable get in user action if player is currently carrying something
	override bool CanBePerformedScript(IEntity user)
	{
		SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(user);
		if (!userChar)
			return false;
		
		SCR_CharacterControllerComponent userCharController = SCR_CharacterControllerComponent.Cast(userChar.GetCharacterController());
		if (!userCharController)
			return false;
		
		if (userCharController.ACE_IsCarrier())
		{
			SetCannotPerformReason("#ACE-UserAction_Carrying");
			return false;
		};
		
		return super.CanBePerformedScript(user);
	}	
}
