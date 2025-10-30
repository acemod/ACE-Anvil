//------------------------------------------------------------------------------------------------
class ACE_Radio_ClearKeysUserAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		ChimeraCharacter userChar = ChimeraCharacter.Cast(user);
		if (!userChar)
			return false;
		
		CharacterControllerComponent userCharController = userChar.GetCharacterController();
		if (!userCharController)
			return false;
		
		return userCharController.GetInspect();
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		ACE_Radio_FillDeviceComponent device = ACE_Radio_FillDeviceComponent.Cast(pOwnerEntity.FindComponent(ACE_Radio_FillDeviceComponent));
		if (!device)
			return;
		
		device.ClearKeys();
		AudioSystem.PlaySound("Sounds/ACE_Radio/beep_low.wav");
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBroadcastScript()
	{
		return false;
	}
}
