//------------------------------------------------------------------------------------------------
//! The reference data shows that the functional form of cook-off times vs T_barrel is close to exponential (A. Hameed et al., Defence Technology 2014, 10, 86–91)
//! Pure Newton's law would predict logarithmic behavior when calculating the time to reach some threshold temperature
//! The solution is to have fast heat exchange coupled with an Arrhenius equation, where cookoff happens when a certain threshold reaction progress was exceeded.
//------------------------------------------------------------------------------------------------
//! TO DO: Reset temperature when new bullet got chambered instead of using OnProjectileShot
class ACE_Overheating_AmmoTemperatureJob : ACE_IFrameJob
{
	protected static ACE_Overheating_Settings s_pSettings;
	protected ref ACE_Overheating_CharacterContext m_pContext = null;
	protected static const float MAX_COOKOFF_RATE_CONSTANT = 303717.68; // [1/s]
	protected static const float COOKOFF_ACTIVATION_SCALE = 18.716667;
	
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
			m_pContext.m_pBarrel.SetCookOffProgress(0);
			return;
		}
		
		float nextTemperature = ComputeNextTemperature(timeSlice);
		if (nextTemperature >= s_pSettings.m_fGunpowderAutoignitionTemperature)
		{
			float cookoffProgress =  ComputeCookOffProgress(timeSlice, nextTemperature);
			
			// Do cook-off if progress has exceeded
			if (cookoffProgress >= m_pContext.m_pBarrel.GetCookOffProgressScale())
			{
				m_pContext.m_pCharCommandHandler.ACE_ForceWeaponFire();
				nextTemperature = ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE;
				cookoffProgress = 0;
				m_pContext.m_pBarrel.InitCookOffCookOffProgressScale();
			}
			
			m_pContext.m_pBarrel.SetCookOffProgress(cookoffProgress);
		}
		
		m_pContext.m_pBarrel.SetAmmoTemperature(nextTemperature);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Computes new bullet temperature by using Newton's law of cooling for heat transfer from barrel
	protected float ComputeNextTemperature(float timeSlice)
	{
		// TO DO: Account for surface area
		float progress = Math.Min(s_pSettings.m_fBarrel2BulletMassScaledTemperatureRateConstant / 1000 / m_pContext.m_pBarrel.GetData().GetBulletMass() * timeSlice, 1); // 1000 for kg to g
		return Math.Lerp(m_pContext.m_pBarrel.GetAmmoTemperature(), m_pContext.m_pBarrel.GetBarrelTemperature(), progress);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Uses Arrhenius equation to compute the progress of some reaction that causes cookoffR
	protected float ComputeCookOffProgress(float timeSlice, float nextTemperature)
	{
		float progress = m_pContext.m_pBarrel.GetCookOffProgress();		
		progress += timeSlice * MAX_COOKOFF_RATE_CONSTANT * ACE_Math.Exp(-COOKOFF_ACTIVATION_SCALE * s_pSettings.m_fGunpowderAutoignitionTemperature / nextTemperature);
		return progress;
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
