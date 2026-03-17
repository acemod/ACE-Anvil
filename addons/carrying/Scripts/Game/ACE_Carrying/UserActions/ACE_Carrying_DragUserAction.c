//------------------------------------------------------------------------------------------------
class ACE_Carrying_DragUserAction : ACE_Carrying_BaseUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!super.CanBePerformedScript(user))
			return false;
		
		SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(user);
		if (!userChar)
			return false;
		
		// Check if user can take crouch stance
		SCR_CharacterControllerComponent userCharController = SCR_CharacterControllerComponent.Cast(userChar.GetCharacterController());
		if (!userCharController)
			return false;
		
		return userCharController.ACE_Carrying_CanDragCasualty(SCR_ChimeraCharacter.Cast(GetOwner()), m_sCannotPerformReason);
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.PerformAction(pOwnerEntity, pUserEntity);
		
		SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!userChar)
			return;
		
		SCR_CharacterControllerComponent userCharController = SCR_CharacterControllerComponent.Cast(userChar.GetCharacterController());
		if (!userCharController)
			return;
		
		userCharController.ACE_Carrying_DragCasualty(SCR_ChimeraCharacter.Cast(pOwnerEntity));
	}
}
