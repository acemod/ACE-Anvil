/*
	Previous State: Charged
	Next State: Cooldown

	Conditions:
	- Must have a patient connected

	- Defibrillator Emulation Notes
		- Automated: Manually triggered via shock action
*/

class ACE_MedicalDefibrillation_ConnectedStateTransition : ACE_FSM_ITransition<ACE_MedicalDefibrillation_DefibrillatorContext>
{
	protected static ACE_MedicalDefibrillation_Defibrillation_Settings s_pDefibrillatorSettings;
	
	//------------------------------------------------------------------------------------------------
	void ACE_MedicalDefibrillation_IDefibState(ACE_FSM_EStateID fromStateIDs, ACE_FSM_EStateID toStateID)
	{
		if (!s_pDefibrillatorSettings)
			s_pDefibrillatorSettings = ACE_SettingsHelperT<ACE_MedicalDefibrillation_Defibrillation_Settings>.GetModSettings();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPerform(ACE_MedicalDefibrillation_DefibrillatorContext context)
	{
		Print("ACE_MedicalDefibrillation_ConnectedStateTransition::OnPerform | Entering state: Connected");
		// Notify that patient has been connected
	}
	
	//------------------------------------------------------------------------------------------------
	override bool ShouldBePerformed(ACE_MedicalDefibrillation_DefibrillatorContext context, float timeSlice)
	{
		IEntity patient = context.m_pDefibrillator.GetPatient();
		if (!patient)
			return false;
		
		return true;
	}
}