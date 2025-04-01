//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_Info")]
class ACE_Captives_ToggleSurrenderContextAction : SCR_SelectedEntitiesContextAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShown(SCR_EditableEntityComponent selectedEntity, vector cursorWorldPosition, int flags)
	{
		if (!super.CanBeShown(selectedEntity, cursorWorldPosition, flags))
			return false;
		
		return SCR_EditableCharacterComponent.Cast(selectedEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed(SCR_EditableEntityComponent selectedEntity, vector cursorWorldPosition, int flags)
	{
		if (!super.CanBePerformed(selectedEntity, cursorWorldPosition, flags))
			return false;
		
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(selectedEntity.GetOwner());
		if (!char)
			return false;
		
		CharacterControllerComponent charController = char.GetCharacterController();
		if (!charController || charController.GetLifeState() != ECharacterLifeState.ALIVE)
			return false;
		
		IEntity parent = selectedEntity.GetOwner().GetParent();
		if (!parent)
			return true;
				
		return ACE_AnimationHelperCompartment.Cast(parent);
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
		
		charController.ACE_Captives_SetSurrender(!charController.ACE_Captives_HasSurrendered());
	}
}
