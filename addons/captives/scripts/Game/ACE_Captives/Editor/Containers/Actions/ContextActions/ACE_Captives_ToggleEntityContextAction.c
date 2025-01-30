//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleUIInfo("m_Info")]
class ACE_Captives_ToggleEntityContextAction : SCR_SelectedEntitiesContextAction
{
	[Attribute(uiwidget: UIWidgets.ResourceNamePicker, desc: "Prefab name of the helper compartment", params: "et")]
	protected ResourceName m_sHelperCompartmentPrefabName;
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShown(SCR_EditableEntityComponent selectedEntity, vector cursorWorldPosition, int flags)
	{
		return SCR_EditableCharacterComponent.Cast(selectedEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed(SCR_EditableEntityComponent selectedEntity, vector cursorWorldPosition, int flags)
	{
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(selectedEntity.GetOwner());
		if (!char)
			return false;
		
		CharacterControllerComponent charCtrl = char.GetCharacterController();
		if (!charCtrl || charCtrl.GetLifeState() != ECharacterLifeState.ALIVE)
			return false;
		
		IEntity parent = selectedEntity.GetOwner().GetParent();
		if (!parent)
			return true;
				
		return ACE_AnimationHelperCompartment.Cast(parent);
	}
	
	//------------------------------------------------------------------------------------------------
	override void Perform(SCR_EditableEntityComponent selectedEntity, vector cursorWorldPosition)
	{
		IEntity char = selectedEntity.GetOwner();
		if (!char)
			return;
		
		ACE_AnimationHelperCompartment helper = ACE_AnimationHelperCompartment.Cast(char.GetParent());
		if (helper && helper.GetPrefabData().GetPrefabName() == m_sHelperCompartmentPrefabName)
			helper.Terminate(EGetOutType.ANIMATED);
		else
			ACE_AnimationTools.AnimateWithHelperCompartment(char, m_sHelperCompartmentPrefabName);
	}
}
