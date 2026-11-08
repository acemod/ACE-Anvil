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
	
	//------------------------------------------------------------------------------------------------
	//! Fix missing assignment of m_pScrInputContext in vanilla implementation
	override bool Execute(IEntity cursorTarget, IEntity groupEnt, vector targetPosition, int playerID, bool isClient)
	{
		if (SCR_PlayerController.GetLocalPlayerId() != playerID)
			return true;
		
		IEntity playerControlledEntity = GetGame().GetPlayerController().GetControlledEntity();		
		if (!playerControlledEntity)
			return false;
		
		SCR_CharacterControllerComponent characterController = SCR_CharacterControllerComponent.Cast(playerControlledEntity.FindComponent(SCR_CharacterControllerComponent));
		if (!characterController)
			return false;
		
		m_pScrInputContext = characterController.GetScrInputContext();
		return super.Execute(cursorTarget, groupEnt, targetPosition, playerID, isClient);
	}
}
