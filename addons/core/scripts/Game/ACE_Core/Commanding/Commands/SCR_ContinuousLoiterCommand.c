//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseGroupCommandTitleField("m_sCommandName")]
modded class SCR_ContinuousLoiterCommand : SCR_BaseRadialCommand
{
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed(notnull SCR_ChimeraCharacter user)
	{
		if (!super.CanBePerformed(user))
			return false;
		
		if (user.IsInVehicle())
			return false;
		
		SCR_CharacterControllerComponent userCharController = SCR_CharacterControllerComponent.Cast(user.GetCharacterController());
		if (!userCharController || userCharController.IsUnconscious() || userCharController.IsSwimming() || userCharController.IsFalling())
			return false;
		
		CharacterCommandHandlerComponent userCommandHandler = user.GetCommandHandler();
		if (!userCommandHandler || userCommandHandler.GetTargetLadder())
			return false;

		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(user.GetCompartmentAccessComponent());
		if (!compartmentAccess || compartmentAccess.ACE_IsGettingIn() || compartmentAccess.ACE_IsGettingOut())
			return false;
		
		return true;
	}
}
