//------------------------------------------------------------------------------------------------
class ACE_Captives_EscortCaptiveUserAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar)
			return false;
		
		if (ownerChar.IsInVehicle() && !ACE_AnimationTools.GetHelperCompartment(ownerChar))
			return false;
		
		SCR_CharacterControllerComponent ownerCharController = SCR_CharacterControllerComponent.Cast(ownerChar.GetCharacterController());
		if (!ownerCharController)
			return false;

		if (ownerCharController.GetLifeState() != ECharacterLifeState.ALIVE)
			return false;
		
		if (ownerCharController.ACE_IsCarried())
			return false;
		
		if (!ownerCharController.ACE_Captives_IsCaptive())
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!super.CanBePerformedScript(user))
			return false;
		
		SCR_CharacterControllerComponent userCharController = SCR_CharacterControllerComponent.Cast(user.FindComponent(SCR_CharacterControllerComponent));
		if (userCharController.ACE_IsCarrier())
		{
			SetCannotPerformReason("#ACE-UserAction_Carrying");
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(pOwnerEntity);
		if (!ownerChar)
			return;
		
		SCR_CharacterControllerComponent ownerCharController = SCR_CharacterControllerComponent.Cast(ownerChar.GetCharacterController());
		if (!ownerCharController)
			return;
		
		ownerCharController.ACE_Captives_EscortCaptive(SCR_ChimeraCharacter.Cast(pUserEntity));
	}
	
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBroadcastScript()
	{
		return false;
	}
}
