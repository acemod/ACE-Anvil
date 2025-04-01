//------------------------------------------------------------------------------------------------
class ACE_Carrying_BaseUserAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar || ownerChar.IsInVehicle())
			return false;
		
		SCR_CharacterControllerComponent ownerCharController = SCR_CharacterControllerComponent.Cast(ownerChar.GetCharacterController());
		if (!ownerCharController || ownerCharController.GetLifeState() != ECharacterLifeState.INCAPACITATED)
			return false;

		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Only run PerformAction on server
	override bool CanBroadcastScript()
	{
		return false;
	}
}
