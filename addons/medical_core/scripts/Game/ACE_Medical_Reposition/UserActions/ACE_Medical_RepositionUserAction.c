//------------------------------------------------------------------------------------------------
class ACE_Medical_RepositionUserAction : ScriptedUserAction
{
	[Attribute(uiwidget: UIWidgets.ComboBox, desc: "Target unconscious pose", enums: ParamEnumArray.FromEnum(ACE_Medical_EUnconsciousPose))]
	protected ACE_Medical_EUnconsciousPose m_eUnconsciousPose;
	
	//------------------------------------------------------------------------------------------------
	//! Can this action be shown in the UI to the provided user entity?
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;

		SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(user);
		if (!userChar)
			return false;

		if (userChar.IsInVehicle())
			return false;

		CharacterControllerComponent userController = userChar.GetCharacterController();
		if (!userController)
			return false;

		if (userController.IsSwimming() || userController.IsFalling())
			return false;
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar)
			return false;
		
		if (ownerChar.IsInVehicle())
			return false;
		
		SCR_CharacterControllerComponent ownerController = SCR_CharacterControllerComponent.Cast(ownerChar.GetCharacterController());
		if (!ownerController)
			return false;
		
		if (!ownerController.IsUnconscious() || ownerController.ACE_Medical_GetUnconsciousPose() == m_eUnconsciousPose)
			return false;
		
		CharacterAnimationComponent ownerAnimComponent = ownerChar.GetAnimationComponent();
		if (!ownerAnimComponent)
			return false;
		
		if (!ownerAnimComponent || ownerAnimComponent.IsRagdollActive())
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(pOwnerEntity);
		if (!ownerChar)
			return;
		
		SCR_CharacterControllerComponent ownerController = SCR_CharacterControllerComponent.Cast(ownerChar.GetCharacterController());
		if (!ownerController)
			return;
		
		ownerController.ACE_Medical_Reposition(m_eUnconsciousPose);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clients should not run the action
	override bool CanBroadcastScript()
	{
		return false;
	}
}
