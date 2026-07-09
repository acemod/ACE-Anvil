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
		Rpc(RplDo_ACE_SetCanHandleWeaponFireOwner, canHandle);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RplDo_ACE_SetCanHandleWeaponFireOwner(bool canHandle)
	{
		m_bACE_CanHandleWeaponFire = canHandle;
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_ForceWeaponFire()
	{
		Rpc(RplDo_ACE_ForceWeaponFireOwner);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RplDo_ACE_ForceWeaponFireOwner()
	{
		m_bACE_ForceWeaponFire = true;
	}
}
