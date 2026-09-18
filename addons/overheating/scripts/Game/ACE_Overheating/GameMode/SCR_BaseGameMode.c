//------------------------------------------------------------------------------------------------
modded class SCR_BaseGameMode : BaseGameMode
{
	//------------------------------------------------------------------------------------------------
	//! Unregister dead players from systems
	override protected void OnPlayerKilled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
	{
		super.OnPlayerKilled(playerId, playerEntity, killerEntity, killer);
		
		SCR_ChimeraCharacter playerChar = SCR_ChimeraCharacter.Cast(playerEntity);
		if (!playerChar)
			return;
		
		EquipedWeaponStorageComponent weaponStorage = EquipedWeaponStorageComponent.Cast(playerChar.FindComponent(EquipedWeaponStorageComponent));
		if (weaponStorage)
			weaponStorage.ACE_Overheating_ToggleRegisterWeapons(false);
		
		ACE_Overheating_AmmoTemperatureSystem ammoSystem = ACE_Overheating_AmmoTemperatureSystem.GetInstance(playerChar.GetWorld());
		if (ammoSystem)
			ammoSystem.Unregister(playerChar);
	}
}
