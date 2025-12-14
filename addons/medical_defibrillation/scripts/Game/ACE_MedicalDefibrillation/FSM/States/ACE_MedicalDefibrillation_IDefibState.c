class ACE_MedicalDefibrillation_IDefibState : ACE_FSM_IState<ACE_MedicalDefibrillation_DefibrillatorContext>
{
	// protected static ACE_MedicalDefibrillation_Settings s_pDefibrillatorSettings;
	protected ACE_Medical_CharacterContext m_pPatient;
	
	//------------------------------------------------------------------------------------------------
	void ACE_MedicalDefibrillation_IDefibState(ACE_FSM_EStateID id)
	{
		// if (!s_pDefibrillatorSettings)
			// s_pDefibrillatorSettings = ACE_SettingsHelperT<ACE_MedicalDefibrillation_Settings>.GetModSettings();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_MedicalDefibrillation_DefibrillatorContext context)
	{
		super.OnEnter(context);
		
		context.m_pDefibrillator.GetDefibProgressData().ResetTimer(ACE_MedicalDefibrillation_EDefibProgressCategory.StateTimeElapsed);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_MedicalDefibrillation_DefibrillatorContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		
		// Update total time in state
		context.m_pDefibrillator.GetDefibProgressData().ModifyTimer(ACE_MedicalDefibrillation_EDefibProgressCategory.StateTimeElapsed, timeSlice);
	}
}