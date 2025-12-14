/*
	Previous State: Analysing
	Next State: Analysed

	Conditions:
	- Must have a patient connected

	- Defibrillator Emulation Notes
		- Manual: Cannot use this state
*/

class ACE_MedicalDefibrillation_AnalysedStateTransition : ACE_FSM_ITransition<ACE_MedicalDefibrillation_DefibrillatorContext>
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
		if (context.m_pDefibrillator.GetDefibrillatorEmulation() == ACE_MedicalDefibrillation_EDefibrillatorEmulation.Manual_NotImplimented)
			return;
		
		Print("ACE_MedicalDefibrillation_AnalysedStateTransition::OnPerform | Entering state: Analysed");
		// Play shock advised/not advised sound
	}
	
	//------------------------------------------------------------------------------------------------
	override bool ShouldBePerformed(ACE_MedicalDefibrillation_DefibrillatorContext context, float timeSlice)
	{
		if (context.m_pDefibrillator.GetDefibrillatorEmulation() == ACE_MedicalDefibrillation_EDefibrillatorEmulation.Manual_NotImplimented)
			return false;
		
		if (!context.m_pDefibrillator.GetPatient())
			return false;
		
		float percentComplete = context.m_pDefibrillator.GetDefibProgressData().GetPercentComplete(ACE_MedicalDefibrillation_EDefibProgressCategory.Analysis);
		if (percentComplete >= 1.0)
			return true;
		
		return false;
	}
}