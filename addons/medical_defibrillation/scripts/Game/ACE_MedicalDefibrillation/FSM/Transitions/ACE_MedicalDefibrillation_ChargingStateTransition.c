/*
	Previous State: Analysed
	Next State: Charging

	Conditions:
	- Must have a patient connected
	- Must be in analysed state for at least 2 seconds (automated)

	- Defibrillator Emulation Notes
		- Manual: Manually enters this state via action
*/

class ACE_MedicalDefibrillation_ChargingStateTransition : ACE_FSM_ITransition<ACE_MedicalDefibrillation_DefibrillatorContext>
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
		Print("ACE_MedicalDefibrillation_ChargingStateTransition::OnPerform | Entering state: Charging");
	}
	
	//------------------------------------------------------------------------------------------------
	override bool ShouldBePerformed(ACE_MedicalDefibrillation_DefibrillatorContext context, float timeSlice)
	{
		if (context.m_pDefibrillator.GetDefibrillatorEmulation() == ACE_MedicalDefibrillation_EDefibrillatorEmulation.Manual_NotImplimented)
			return false;
		
		IEntity patient = context.m_pDefibrillator.GetPatient();
		if (!patient)
			return false;
		
		if (context.m_pDefibrillator.GetDefibProgressData().GetTimer(ACE_MedicalDefibrillation_EDefibProgressCategory.StateTimeElapsed) >= 2.0)
			return true;
		
		return false;
	}
}