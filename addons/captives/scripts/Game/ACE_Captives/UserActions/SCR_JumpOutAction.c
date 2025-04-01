//------------------------------------------------------------------------------------------------
//! Disable action for captives
modded class SCR_JumpOutAction : SCR_CompartmentUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;
		
		return !ACE_Captive_IsUserCaptive(user);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!super.CanBePerformedScript(user))
			return false;
		
		return !ACE_Captive_IsUserCaptive(user);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool ACE_Captive_IsUserCaptive(IEntity user)
	{
		SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(user);
		if (!userChar)
			return false;
		
		SCR_CharacterControllerComponent userCharController = SCR_CharacterControllerComponent.Cast(userChar.GetCharacterController());
		if (!userCharController)
			return false;
		
		return userCharController.ACE_Captives_IsCaptive();
	}
}