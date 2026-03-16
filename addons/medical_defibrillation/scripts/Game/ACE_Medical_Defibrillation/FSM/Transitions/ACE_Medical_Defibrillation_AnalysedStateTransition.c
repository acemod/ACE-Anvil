/*
	Previous State: Analysing
	Next State: Analysed

	Conditions:
	- Must have a patient connected

	- Defibrillator Emulation Notes
		- Manual: Cannot use this state
*/

class ACE_Medical_Defibrillation_AnalysedStateTransition : ACE_FSM_ITransition<ACE_Medical_Defibrillation_DefibContext>
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
		if (context.m_pDefibrillator.GetDefibrillatorEmulation() == ACE_Medical_Defibrillation_EDefibEmulation.Manual_NotImplimented)
			return;
		
		Print("ACE_Medical_Defibrillation_AnalysedStateTransition::OnPerform | Entering state: Analysed");
	}
	
	//------------------------------------------------------------------------------------------------
	override bool ShouldBePerformed(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		if (context.m_pDefibrillator.GetDefibrillatorEmulation() == ACE_Medical_Defibrillation_EDefibEmulation.Manual_NotImplimented)
			return false;
		
		if (!context.m_pDefibrillator.GetPatient())
			return false;
		
		float percentComplete = context.m_pDefibrillator.GetDefibProgressData().GetPercentComplete(ACE_Medical_Defibrillation_EDefibProgressCategory.Analysis);
		if (percentComplete >= 1.0)
		{
			// Wait for no sound to be playing
			if (!context.m_pDefibrillator.GetSoundComponent().IsPlaying())
				return true;
		}

		return false;
	}
}