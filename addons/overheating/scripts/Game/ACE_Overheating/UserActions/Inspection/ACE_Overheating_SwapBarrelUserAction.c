//------------------------------------------------------------------------------------------------
class ACE_Overheating_SwapBarrelUserAction : SCR_InspectionUserAction
{
	bool m_bSecondSoundPlayed;
	
	//------------------------------------------------------------------------------------------------
	override protected void PerformActionInternal(SCR_InventoryStorageManagerComponent manager, IEntity pOwnerEntity, IEntity pUserEntity)
	{		
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (playerController)
			playerController.ACE_Overheating_RequestSwapBarrel(m_WeaponComponent);
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
