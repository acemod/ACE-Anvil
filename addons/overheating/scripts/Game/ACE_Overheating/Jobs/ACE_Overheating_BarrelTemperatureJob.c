//------------------------------------------------------------------------------------------------
class ACE_Overheating_BarrelTemperatureJob : ACE_IFrameJob
{
	protected static ACE_Overheating_Settings s_pSettings;
	protected static BaseWeatherManagerEntity s_pWeatherManager;
	protected ref ACE_Overheating_BarrelContext m_pContext = null;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Overheating_BarrelTemperatureJob()
	{
		if (!s_pSettings)
			s_pSettings = ACE_SettingsHelperT<ACE_Overheating_Settings>.GetModSettings();
		
		if (!s_pWeatherManager)
		{
			ChimeraWorld world = GetGame().GetWorld();
			s_pWeatherManager = world.GetTimeAndWeatherManager();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnStart()
	{
		super.OnStart();
		
		if (!m_pContext.IsValid())
			return;
		
		OnUpdate((GetGame().GetWorld().GetWorldTime() - m_pContext.m_pObject.GetLastTemperatureTimestamp()) / 1000);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnStop()
	{
		super.OnStop();
		
		if (!m_pContext.IsValid())
			return;
		
		m_pContext.m_pObject.SetLastTemperatureTimestamp(GetGame().GetWorld().GetWorldTime());
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(float timeSlice)
	{
		super.OnUpdate(timeSlice);
		
		if (!m_pContext.IsValid())
			return;
				
		// Temporary solution: Use standard ambient temperature until we got a proper weather system
		float externalTemperature = ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE;
		float currentTemperature = m_pContext.m_pObject.GetBarrelTemperature();
		float nextTemperature = currentTemperature;
		nextTemperature += (ComputeHeating(timeSlice) + ComputeCooling(timeSlice, currentTemperature, externalTemperature)) / m_pContext.m_pObject.GetData().GetBarrelHeatCapacity();		
		nextTemperature = Math.Max(nextTemperature, externalTemperature);
		m_pContext.m_pObject.SetBarrelTemperature(nextTemperature);
		m_pContext.m_pObject.SetJamChance(ComputeJamChance(nextTemperature));
		
		if (m_pContext.m_pHelperAttachment)
			m_pContext.m_pHelperAttachment.UpdateStats(ComputeMuzzleDispersionFactor(nextTemperature));
		
		if (m_pContext.m_pGlowEffect)
			m_pContext.m_pGlowEffect.SetIntensity(Math.Clamp(Math.InverseLerp(s_pSettings.m_fMinGlowTemperature, 1300, nextTemperature), 0, 1));
		
		if (m_pContext.m_pSmokeEffect)
			m_pContext.m_pSmokeEffect.UpdateParams(nextTemperature);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns the accumulated heat by fired projectiles
	protected float ComputeHeating(float timeSlice)
	{
		return s_pSettings.m_fHeatingScale * m_pContext.m_pObject.PopAccumulatedHeat();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns the heat loss of the barrel through convection and radiation
	protected float ComputeCooling(float timeSlice, float currentTemperature, float externalTemperature)
	{
		// Conductive and convective heat flux (Newton's law of cooling)
		float heatFlux = -ComputeHeatTransferCoefficient() * (currentTemperature - externalTemperature);
		// Radidative heat flux (Stefan-Boltzmann law)
		heatFlux -= m_pContext.m_pObject.GetData().GetBarrelEmissivity() * ACE_PhysicalConstants.STEFAN_BOLTZMANN * (Math.Pow(currentTemperature, 4) - Math.Pow(externalTemperature, 4));
		return heatFlux * m_pContext.m_pObject.GetData().GetBarrelSurfaceArea() * timeSlice;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns heat transfer coefficient in W / (m^2 * K)
	//------------------------------------------------------------------------------------------------
	//! Equation (1):  Nu = 0.26 * Re^0.6 * Pr^0.37 for Re in [1000, 20000]
	//! Reference: A. Žukauskas, Adv. Heat Transfer 1972, 8, 131.
	//------------------------------------------------------------------------------------------------
	//! AIR
	//! ---
	//! Relation: h = 4.5908 * v^0.6 / d^0.4
	//!
	//! Derived from equation (1) with
	//! - Pr(air) = 0.714672
	//! - k(air) = 0.02614 W/(m*K)
	//! - η(air) = 1.846e-5 kg/(m*s)
	//! - ρ(air) = 1.1839 kg/m^3
	//------------------------------------------------------------------------------------------------
	//! RAIN
	//! ----
	//! Relation: h = RI * 9.3198 / d^0.4
	//!
	//! Derived from equation (1), such that h = 50 for rain intensity (RI) = 1 and d = 0.015 m
	//------------------------------------------------------------------------------------------------
	//! TO DO: Account for water and better relation for rain
	protected float ComputeHeatTransferCoefficient()
	{
		vector airVelocity = s_pWeatherManager.GetWindSpeed() * vector.FromYaw(s_pWeatherManager.GetWindDirection());
		
		if (m_pContext.m_pOwnerChar && !m_pContext.m_pOwnerChar.IsInVehicle())
			airVelocity += m_pContext.m_pOwnerChar.GetPhysics().GetVelocity();
		
		float h = 4.5908 * Math.Pow(airVelocity.LengthSq(), 3.0 / 10.0) + 9.3198 * s_pWeatherManager.GetRainIntensity();
		h /= Math.Pow(m_pContext.m_pObject.GetData().GetBarrelDiameter(), 0.4);
		h = Math.Max(12.5, h);
		
		if (m_pContext.m_bIsChamberingPossible)
			h *= s_pSettings.m_fDefaultAirCoolingScale;
		else
			h *= s_pSettings.m_fOpenBoltAirCoolingScale;
		
		h += s_pSettings.m_fBaseHeatTransferCoefficient;
		
	#ifdef ENABLE_DIAG
		m_pContext.m_pObject.SetHeatTransferCoefficient(h);
	#endif
		
		return h;
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeJamChance(float temperature)
	{
		return s_pSettings.m_fJamChanceScale * m_pContext.m_pObject.GetData().ComputeJamChance(temperature);
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeMuzzleDispersionFactor(float temperature)
	{
		return 1 + s_pSettings.m_fMuzzleDispersionScale * m_pContext.m_pObject.GetData().ComputeAdditionalMuzzleDispersionFactor(temperature);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetContext(ACE_Overheating_BarrelContext context)
	{
		m_pContext = context;
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Overheating_BarrelContext GetContext()
	{
		return m_pContext;
	}
}
