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
		return s_pCirculationSettings.m_fDefaultStrokeVolumeML * context.m_pBloodHitZone.GetHealthScaled();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Treated as constant, except for medication
	protected float ComputeSystemicVascularResistance(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return s_pCirculationSettings.m_fDefaultSystemicVascularResistance + context.m_pVitals.GetSystemicVascularResistenceMedicationAdjustment();
	}
	
	//------------------------------------------------------------------------------------------------
	//! MAP = CO * SVR
	protected float ComputeMeanArterialPressure(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return context.m_pVitals.GetCardiacOutput() * context.m_pVitals.GetSystemicVascularResistance();
	}
	
	//------------------------------------------------------------------------------------------------
	//! PP ~ MAP
	protected float ComputePulsePressure(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return s_pCirculationSettings.m_fPulsePressureScale * context.m_pVitals.GetMeanArterialPressure();
	}
}
