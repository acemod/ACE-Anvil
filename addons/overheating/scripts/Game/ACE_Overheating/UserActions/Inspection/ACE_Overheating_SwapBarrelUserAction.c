//------------------------------------------------------------------------------------------------
class ACE_Overheating_SwapBarrelUserAction : SCR_InventoryAction
{
	bool m_bSecondSoundPlayed;
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		ChimeraCharacter userChar = ChimeraCharacter.Cast(user);
		if(!userChar)
			return false;

		CharacterControllerComponent userCharController = userChar.GetCharacterController();
		if(!userCharController)
			return false;

		if (!userCharController.GetInspect())
			return false;
		
		BaseWeaponComponent weapon = userChar.GetWeaponManager().GetCurrentWeapon();
		if (!weapon)
			return false;
		
		return (weapon.GetWeaponType() == EWeaponType.WT_MACHINEGUN);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void PerformActionInternal(SCR_InventoryStorageManagerComponent manager, IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
			return;

		ChimeraCharacter userChar = ChimeraCharacter.Cast(playerController.GetLocalControlledEntity());
		if (!userChar)
			return;

		playerController.ACE_Overheating_RequestSwapBarrel(userChar.GetWeaponManager().GetCurrentWeapon(), GetOwner());
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnActionStart(IEntity pUserEntity)
	{
		SCR_UISoundEntity.SoundEvent("ACE_OVERHEATING_SOUND_SWAP_BARREL");
		m_bSecondSoundPlayed = false;
	}
	
	//------------------------------------------------------------------------------------------------
	override float GetActionProgressScript(float fProgress, float timeSlice)
	{
		float progress = super.GetActionProgressScript(fProgress, timeSlice);
		
		if (!m_bSecondSoundPlayed && GetActionDuration() - progress < 2)
		{
			SCR_UISoundEntity.SoundEvent("ACE_OVERHEATING_SOUND_SWAP_BARREL");
			m_bSecondSoundPlayed = true;
		}
		
		return progress;
	}
}
