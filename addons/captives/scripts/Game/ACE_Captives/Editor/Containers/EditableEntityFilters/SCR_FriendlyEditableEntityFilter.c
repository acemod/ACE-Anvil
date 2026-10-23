//------------------------------------------------------------------------------------------------
//! This filter is used for deciding whether tags are shown in armavision
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(EEditableEntityState, "m_State")]
modded class SCR_FriendlyEditableEntityFilter : SCR_BaseEditableEntityFilter
{
	//------------------------------------------------------------------------------------------------
	//! Return false if local player is captive or has surrenders, such that they can't see any tags
	override protected bool IsFriendly(GenericEntity owner)
	{
		if (!super.IsFriendly(owner))
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
