//------------------------------------------------------------------------------------------------
class ACE_Medical_IVitalState : ACE_FSM_IState<ACE_Medical_CharacterContext>
{
	protected static ACE_Medical_Circulation_Settings s_pCirculationSettings;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_IVitalState(ACE_FSM_EStateID id)
	{
		if (!s_pCirculationSettings)
			s_pCirculationSettings = ACE_SettingsHelperT<ACE_Medical_Circulation_Settings>.GetModSettings();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update resiliance scale for new state
	override void OnEnter(ACE_Medical_CharacterContext context)
	{
		super.OnEnter(context);
		context.m_pDamageManager.ACE_Medical_UpdateResilienceRegenScale();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update vitals
	override void OnUpdate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		context.m_pVitals.SetHeartRate(ComputeHeartRate(context, timeSlice));
		context.m_pVitals.SetCardiacOutput(ComputeCardiacOutput(context, timeSlice));
		context.m_pVitals.SetSystemicVascularResistance(ComputeSystemicVascularResistance(context, timeSlice));
		context.m_pVitals.SetMeanArterialPressure(ComputeMeanArterialPressure(context, timeSlice));
		context.m_pVitals.SetPulsePressure(ComputePulsePressure(context, timeSlice));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Implement the heart rate simulation here
	protected float ComputeHeartRate(ACE_Medical_CharacterContext context, float timeSlice);
	
	//------------------------------------------------------------------------------------------------
	//! CO = HR * SV
	protected float ComputeCardiacOutput(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return context.m_pVitals.GetHeartRate() * ComputeStrokeVolume(context, timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeStrokeVolume(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float healthScaled = context.m_pBloodHitZone.GetHealthScaled();
		// If healthScaled is 0 or invalid, use 1.0 (full health) to prevent calculation collapse
		// This ensures BP calculations work even if hit zones aren't fully initialized yet
		if (healthScaled <= 0)
			healthScaled = 1.0;
		
		float defaultSV = s_pCirculationSettings.m_fDefaultStrokeVolumeML;
		float computedSV = defaultSV * healthScaled;
		return computedSV;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Treated as constant, except for medication
	protected float ComputeSystemicVascularResistance(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float baseSVR = s_pCirculationSettings.m_fDefaultSystemicVascularResistance;
		// Ensure settings Init() has been called - calculate derived value if needed
		if (baseSVR == 0 && s_pCirculationSettings)
		{
			s_pCirculationSettings.m_fDefaultSystemicVascularResistance = s_pCirculationSettings.m_fDefaultMeanArterialPressureKPA / (s_pCirculationSettings.m_fDefaultHeartRateBPM * s_pCirculationSettings.m_fDefaultStrokeVolumeML);
			baseSVR = s_pCirculationSettings.m_fDefaultSystemicVascularResistance;
		}
		return baseSVR + context.m_pVitals.GetSystemicVascularResistenceMedicationAdjustment();
	}
	
	//------------------------------------------------------------------------------------------------
	//! MAP = CO * SVR
	protected float ComputeMeanArterialPressure(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float co = context.m_pVitals.GetCardiacOutput();
		float svr = context.m_pVitals.GetSystemicVascularResistance();
		float mapValue = co * svr;
		return mapValue;
	}
	
	//------------------------------------------------------------------------------------------------
	//! PP ~ MAP
	protected float ComputePulsePressure(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float pulsePressureScale = s_pCirculationSettings.m_fPulsePressureScale;
		// Ensure settings Init() has been called - calculate derived value if needed
		if (pulsePressureScale == 0 && s_pCirculationSettings)
		{
			s_pCirculationSettings.m_fPulsePressureScale = s_pCirculationSettings.m_fDefaultPulsePressureKPA / s_pCirculationSettings.m_fDefaultMeanArterialPressureKPA;
			pulsePressureScale = s_pCirculationSettings.m_fPulsePressureScale;
		}
		
		return pulsePressureScale * context.m_pVitals.GetMeanArterialPressure();
	}
}
