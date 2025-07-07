//------------------------------------------------------------------------------------------------
//! Add flag for enabling/disable HandleWeaponFire
modded class SCR_CharacterCommandHandlerComponent : CharacterCommandHandlerComponent
{
	protected bool m_bACE_CanHandleWeaponFire = true;
	protected bool m_bACE_ForceWeaponFire = false;
	
	//------------------------------------------------------------------------------------------------
	override bool HandleWeaponFire(CharacterInputContext pInputCtx, float pDt, int pCurrentCommandID)
	{
		if (!m_bACE_CanHandleWeaponFire)
			return false;
		
		if (m_bACE_ForceWeaponFire)
		{
			if (pInputCtx.WeaponIsRaised())
				pInputCtx.SetWeaponPullTrigger(true);
			
			m_bACE_ForceWeaponFire = false;
		}
		
		return HandleWeaponFireDefault(pInputCtx, pDt, pCurrentCommandID);
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_SetCanHandleWeaponFire(bool canHandle)
	{
		m_bACE_CanHandleWeaponFire = canHandle;
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_ForceWeaponFire()
	{
		m_bACE_ForceWeaponFire = true;
	}
}
