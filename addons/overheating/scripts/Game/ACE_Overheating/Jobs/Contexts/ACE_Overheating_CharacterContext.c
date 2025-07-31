//------------------------------------------------------------------------------------------------
class ACE_Overheating_CharacterContext : ACE_FrameJobScheduler_IObjectContext<SCR_ChimeraCharacter>
{
	ACE_Overheating_MuzzleJamComponent m_pBarrel;
	BaseMuzzleComponent m_pMuzzle;
	SCR_CharacterCommandHandlerComponent m_pCharCommandHandler;
	bool m_bIsChamberingPossible;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Overheating_CharacterContext(SCR_ChimeraCharacter object)
	{
		BaseWeaponManagerComponent weaponManager = object.GetWeaponManager();
		if (weaponManager)
			OnWeaponChanged(weaponManager.GetCurrentWeapon(), null);
				
		EventHandlerManagerComponent eventHandlerManager = EventHandlerManagerComponent.Cast(object.FindComponent(EventHandlerManagerComponent));
		if (eventHandlerManager)
		{
			eventHandlerManager.RegisterScriptHandler("OnWeaponChanged", this, OnWeaponChanged);
			eventHandlerManager.RegisterScriptHandler("OnMuzzleChanged", this, OnMuzzleChanged);
		}
		
		m_pCharCommandHandler = SCR_CharacterCommandHandlerComponent.Cast(object.GetCommandHandler());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update fire handler when weapon changed
	protected void OnWeaponChanged(BaseWeaponComponent newWeapon, BaseWeaponComponent prevWeapon)
	{
		m_pBarrel = ACE_Overheating_MuzzleJamComponent.FromWeapon(newWeapon);
		
		m_pMuzzle = null;
		if (m_pBarrel)
			m_pMuzzle = m_pBarrel.GetMuzzle();
		
		if (m_pMuzzle)
			m_bIsChamberingPossible = m_pMuzzle.IsChamberingPossible();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update fire handler when muzzle changed
	protected void OnMuzzleChanged(BaseWeaponComponent weapon, BaseMuzzleComponent newMuzzle, BaseMuzzleComponent prevMuzzle)
	{
		m_pBarrel = ACE_Overheating_MuzzleJamComponent.FromMuzzle(newMuzzle);
		
		m_pMuzzle = newMuzzle;
		if (m_pMuzzle)
			m_bIsChamberingPossible = m_pMuzzle.IsChamberingPossible();
	}
	
	//------------------------------------------------------------------------------------------------
	void ~ACE_Overheating_CharacterContext()
	{
		if (!m_pObject)
			return;
		
		EventHandlerManagerComponent eventHandlerManager = EventHandlerManagerComponent.Cast(m_pObject.FindComponent(EventHandlerManagerComponent));
		if (eventHandlerManager)
		{
			eventHandlerManager.RemoveScriptHandler("OnWeaponChanged", this, OnWeaponChanged);
			eventHandlerManager.RemoveScriptHandler("OnMuzzleChanged", this, OnMuzzleChanged);
		}
	}
}
