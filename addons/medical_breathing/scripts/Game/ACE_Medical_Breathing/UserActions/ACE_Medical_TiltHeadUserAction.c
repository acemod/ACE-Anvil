//------------------------------------------------------------------------------------------------
class ACE_Medical_TiltHeadUserAction : ScriptedUserAction
{
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
		
		if (ownerController.GetLifeState() != ECharacterLifeState.INCAPACITATED || ownerController.ACE_Medical_GetUnconsciousPose() != ACE_Medical_EUnconsciousPose.BACK)
			return false;
		
		CharacterAnimationComponent ownerAnimComponent = ownerChar.GetAnimationComponent();
		if (!ownerAnimComponent)
			return false;
		
		if (ownerAnimComponent.IsRagdollActive())
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
		
		ownerController.ACE_Medical_TiltHead();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clients should not run the action
	override bool CanBroadcastScript()
	{
		return false;
	}
}
