//------------------------------------------------------------------------------------------------
class ACE_Medical_IVitalState : ACE_FSM_IState<ACE_Medical_VitalStates_CharacterContext>
{
	protected static ACE_Medical_Circulation_Settings s_pCirculationSettings;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_IVitalState(ACE_FSM_EStateID id)
	{
		if (!s_pCirculationSettings)
			s_pCirculationSettings = ACE_SettingsHelperT<ACE_Medical_Circulation_Settings>.GetModSettings();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update state ID on the component
	override void OnEnter(ACE_Medical_VitalStates_CharacterContext context)
	{
		super.OnEnter(context);
		context.m_pComponent.SetVitalState(GetID());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update vitals
	override void OnUpdate(ACE_Medical_VitalStates_CharacterContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		context.m_pComponent.SetHeartRate(ComputeHeartRate(context, timeSlice));
		context.m_pComponent.SetCardiacOutput(ComputeCardiacOutput(context, timeSlice));
		context.m_pComponent.SetSystemicVascularResistance(ComputeSystemicVascularResistance(context, timeSlice));
		context.m_pComponent.SetMeanArterialPressure(ComputeMeanArterialPressure(context, timeSlice));
		context.m_pComponent.SetPulsePressure(ComputePulsePressure(context, timeSlice));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Implement the heart rate simulation here
	protected float ComputeHeartRate(ACE_Medical_VitalStates_CharacterContext context, float timeSlice);
	
	//------------------------------------------------------------------------------------------------
	//! CO = HR * SV
	protected float ComputeCardiacOutput(ACE_Medical_VitalStates_CharacterContext context, float timeSlice)
	{
		return context.m_pComponent.GetHeartRate() * s_pCirculationSettings.m_fDefaultStrokeVolumeML * context.m_pBloodHitZone.GetHealthScaled();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Treated as constant, except for medication
	protected float ComputeSystemicVascularResistance(ACE_Medical_VitalStates_CharacterContext context, float timeSlice)
	{
		/***** return s_pCirculationSettings.m_fDefaultSystemicVascularResistance + context.m_pComponent.GetSystemicVascularResistenceMedicationAdjustment(); *****/
	}
	
	//------------------------------------------------------------------------------------------------
	//! MAP = CO * SVR
	protected float ComputeMeanArterialPressure(ACE_Medical_VitalStates_CharacterContext context, float timeSlice)
	{
		return context.m_pComponent.GetCardiacOutput() * context.m_pComponent.GetSystemicVascularResistance();
	}
	
	//------------------------------------------------------------------------------------------------
	//! PP ~ MAP
	protected float ComputePulsePressure(ACE_Medical_VitalStates_CharacterContext context, float timeSlice)
	{
		return s_pCirculationSettings.m_fPulsePressureScale * context.m_pComponent.GetMeanArterialPressure();
	}
}
