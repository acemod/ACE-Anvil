//------------------------------------------------------------------------------------------------
//! TO DO: Reset temperature when new bullet got chambered instead of using OnProjectileShot
class ACE_Overheating_AmmoTemperatureJob : ACE_IFrameJob
{
	protected static ACE_Overheating_Settings s_pSettings;
	protected ref ACE_Overheating_CharacterContext m_pContext = null;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Overheating_AmmoTemperatureJob()
	{
		if (!s_pSettings)
			s_pSettings = ACE_SettingsHelperT<ACE_Overheating_Settings>.GetModSettings();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(float timeSlice)
	{
		super.OnUpdate(timeSlice);
		
		if (!m_pContext.IsValid() || !m_pContext.m_pBarrel)
			return;
		
		// Reset temperature when jammed or when chamber empty
		if (m_pContext.m_pBarrel.IsJammed() || (m_pContext.m_bIsChamberingPossible && !m_pContext.m_pMuzzle.IsCurrentBarrelChambered()))
		{
			m_pContext.m_pBarrel.SetAmmoTemperature(ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE);
			return;
		}
		
		// TO DO: Account for surface area
		float progress = Math.Min(s_pSettings.m_fBarrel2BulletMassScaledTemperatureRateConstant / 1000 / m_pContext.m_pBarrel.GetData().GetBulletMass() * timeSlice, 1); // 1000 for kg to g
		float nextTemperature = Math.Lerp(m_pContext.m_pBarrel.GetAmmoTemperature(), m_pContext.m_pBarrel.GetBarrelTemperature(), progress);
		
		// Do cook-off if temperature has exceeded
		if (nextTemperature > s_pSettings.m_fGunpowderAutoignitionTemperature)
		{
			m_pContext.m_pCharCommandHandler.ACE_ForceWeaponFire();
			nextTemperature = ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE;
		}
		
		m_pContext.m_pBarrel.SetAmmoTemperature(nextTemperature);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetContext(ACE_Overheating_CharacterContext context)
	{
		m_pContext = context;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Overheating_CharacterContext GetContext()
	{
		return m_pContext;
	}
}
