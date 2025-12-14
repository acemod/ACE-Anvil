/*
	Previous State: Charged
	Next State: Cooldown

	Conditions:
	- Must have a patient connected

	- Defibrillator Emulation Notes
		- Automated: Manually triggered via shock action
*/

class ACE_Medical_Defibrillation_ConnectedStateTransition : ACE_FSM_ITransition<ACE_Medical_Defibrillation_DefibContext>
{
	protected static ACE_Medical_Defibrillation_Defibrillation_Settings s_pDefibrillatorSettings;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_Defibrillation_IDefibState(ACE_FSM_EStateID fromStateIDs, ACE_FSM_EStateID toStateID)
	{
		if (!s_pDefibrillatorSettings)
			s_pDefibrillatorSettings = ACE_SettingsHelperT<ACE_Medical_Defibrillation_Defibrillation_Settings>.GetModSettings();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPerform(ACE_Medical_Defibrillation_DefibContext context)
	{
		Print("ACE_Medical_Defibrillation_ConnectedStateTransition::OnPerform | Entering state: Connected");
		
		context.m_pDefibrillator.GetSoundComponent().PlayDefibSoundServer(ACE_Medical_Defibrillation_DefibSounds.SOUNDCONNECTED);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool ShouldBePerformed(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		IEntity patient = context.m_pDefibrillator.GetPatient();
		if (!patient)
			return false;
		
		return true;
	}
}