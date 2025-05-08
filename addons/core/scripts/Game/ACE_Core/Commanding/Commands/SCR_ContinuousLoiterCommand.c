//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseGroupCommandTitleField("m_sCommandName")]
modded class SCR_ContinuousLoiterCommand : SCR_BaseRadialCommand
{
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed(notnull SCR_ChimeraCharacter user)
	{
		if (!super.CanBePerformed(user))
			return false;
		
		SCR_CharacterControllerComponent userCharController = SCR_CharacterControllerComponent.Cast(user.GetCharacterController());
		if (!userCharController)
			return false;
		
		CharacterCommandHandlerComponent userCommandHandler = user.GetCommandHandler();
		if (!userCommandHandler)
			return false;
		
		return !userCharController.IsUnconscious()
			&& !userCharController.IsSwimming()
			&& !userCharController.IsFalling()
			&& !userCommandHandler.GetTargetLadder()
			&& !user.IsInVehicle();
	}
}
