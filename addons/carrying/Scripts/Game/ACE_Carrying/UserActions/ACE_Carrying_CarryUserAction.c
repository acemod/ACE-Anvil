//------------------------------------------------------------------------------------------------
class ACE_Carrying_CarryUserAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		ChimeraCharacter ownerChar = ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar)
			return false;
		
		SCR_CharacterControllerComponent ownerCharCtrl = SCR_CharacterControllerComponent.Cast(ownerChar.GetCharacterController());
		if (!ownerCharCtrl || !ownerCharCtrl.IsUnconscious() || ownerCharCtrl.GetLifeState() == ECharacterLifeState.DEAD)
			return false;

		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		ChimeraCharacter userChar = ChimeraCharacter.Cast(user);
		if (!userChar)
			return false;
		
		// Check if user can take crouch stance
		SCR_CharacterControllerComponent userCharCtrl = SCR_CharacterControllerComponent.Cast(userChar.GetCharacterController());
		if (!userCharCtrl || !userCharCtrl.CanChangeStance(ECharacterStanceChange.STANCECHANGE_TOCROUCH))
		{
			SetCannotPerformReason("#AR-Keybind_StanceProne");
			return false;
		};
		
		// Check if carrying is already in progress
		if (ACE_Carrying_Tools.IsCarrier(user) || ACE_Carrying_Tools.IsCarried(GetOwner()))
		{
			SetCannotPerformReason("#ACE_Carrying-UserAction_Carrying");
			return false;
		};
			
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		ACE_Carrying_Tools.Carry(pUserEntity, pOwnerEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Methods are executed on the local player
	override bool CanBroadcastScript() { return false; };
};
