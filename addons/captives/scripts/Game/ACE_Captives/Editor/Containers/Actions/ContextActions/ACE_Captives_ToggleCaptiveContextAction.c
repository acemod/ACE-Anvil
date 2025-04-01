//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_Info")]
class ACE_Captives_ToggleCaptiveContextAction : SCR_SelectedEntitiesContextAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShown(SCR_EditableEntityComponent selectedEntity, vector cursorWorldPosition, int flags)
	{
		if (!super.CanBeShown(selectedEntity, cursorWorldPosition, flags))
			return false;
		
		return SCR_EditableCharacterComponent.Cast(selectedEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	override void Perform(SCR_EditableEntityComponent selectedEntity, vector cursorWorldPosition)
	{
		super.Perform(selectedEntity, cursorWorldPosition);
		
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(selectedEntity.GetOwner());
		if (!char)
			return;
		
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(char.GetCharacterController());
		if (!charController)
			return;
		
		charController.ACE_Captives_SetCaptive(!charController.ACE_Captives_IsCaptive());
	}
}
