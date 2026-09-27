//------------------------------------------------------------------------------------------------
[BaseContainerProps("", "Entry of one command"), SCR_BaseGroupCommandTitleField("m_sCommandName")]
class ACE_MetalClangingCommand : SCR_BaseRadialCommand
{
	protected static ref const array<EWeaponType> REQUIRED_WEAPON_TYPES = {EWeaponType.WT_RIFLE, EWeaponType.WT_SNIPERRIFLE, EWeaponType.WT_MACHINEGUN};
	
	//------------------------------------------------------------------------------------------------
	override bool Execute(IEntity cursorTarget, IEntity groupEnt, vector targetPosition, int playerID, bool isClient)
	{
		if (isClient)
			return false;
		
		IEntity char = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerID);
		if (!char)
			return false;
		
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(char.FindComponent(SCR_CharacterControllerComponent));
		if (!charController)
			return false;
		
		charController.ACE_PlaySoundEvent(ACE_ECharacterSoundEvent.ACE_SOUND_METAL_CLANGING);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed(notnull SCR_ChimeraCharacter user)
	{
		if (!super.CanBePerformed(user))
			return false;
		
		CharacterControllerComponent charController = user.GetCharacterController();
		if (!charController || charController.IsUnconscious() || charController.IsSwimming() || charController.IsFalling())
			return false;
		
		BaseWeaponManagerComponent weaponManager = user.GetWeaponManager();
		if (!weaponManager)
			return false;
		
		array<BaseWeaponComponent> weapons = {};
		weaponManager.GetWeapons(weapons);
		
		foreach (BaseWeaponComponent weapon : weapons)
		{
			if (REQUIRED_WEAPON_TYPES.Contains(weapon.GetWeaponType()))
				return true;
		}
		
		return false;
	}
}
