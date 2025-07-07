//------------------------------------------------------------------------------------------------
class ACE_Overheating_RemoveMagFailed : ACE_FSM_ITransition<ACE_Overheating_WeaponAnimContext>
{
	//------------------------------------------------------------------------------------------------
	override bool ShouldBePerformed(ACE_Overheating_WeaponAnimContext context)
	{
		// Can return false the first few frames, so we compensate with m_bIsStartRemovingMag
		if (context.m_bIsStartRemovingMag || context.IsReloading())
			return false;
		
		return context.m_pWeapon.GetCurrentMagazine();
	}
}
