//------------------------------------------------------------------------------------------------
//! Prevent captive and surrendered players from seeing name tags
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(EEditableEntityState, "m_State")]
modded class SCR_FriendlyPlayerEditableEntityFilter : SCR_PlayerEditableEntityFilter
{
	//------------------------------------------------------------------------------------------------
	override bool CanAdd(SCR_EditableEntityComponent entity)
	{
		if (!super.CanAdd(entity))
			return false;
		
		SCR_ChimeraCharacter localPlayer = SCR_ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		if (!localPlayer)
			return false;
		
		SCR_CharacterControllerComponent localCharController = SCR_CharacterControllerComponent.Cast(localPlayer.GetCharacterController());
		if (!localCharController)
			return false;
		
		return (!localCharController.ACE_Captives_IsCaptive() && !localCharController.ACE_Captives_HasSurrendered());
	}
}
