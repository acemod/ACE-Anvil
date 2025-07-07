//------------------------------------------------------------------------------------------------
class ACE_Overheating_RemoveMagCompleted : ACE_FSM_ITransition<ACE_Overheating_WeaponAnimContext>
{
	//------------------------------------------------------------------------------------------------
	override bool ShouldBePerformed(ACE_Overheating_WeaponAnimContext context)
	{
		if (context.m_bIsStartRemovingMag || context.IsReloading() || context.m_pWeapon.GetCurrentMagazine())
			return false;
		
		return true;
	}
}
