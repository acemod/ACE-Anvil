/*
	Previous State: Charging
	Next State: Charged

	Conditions:
	- Must have a patient connected
	- Must have charge percentage >= 1
*/

class ACE_Medical_Defibrillation_ChargedStateTransition : ACE_FSM_ITransition<ACE_Medical_Defibrillation_DefibContext>
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
		Print("ACE_Medical_Defibrillation_ChargedStateTransition::OnPerform | Entering state: Charged");
	}
	
	//------------------------------------------------------------------------------------------------
	override bool ShouldBePerformed(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		if (context.m_pDefibrillator.GetDefibrillatorEmulation() == ACE_Medical_Defibrillation_EDefibEmulation.Manual_NotImplimented)
			return false;
		
		IEntity patient = context.m_pDefibrillator.GetPatient();
		if (!patient)
			return false;
		
		if (context.m_pDefibrillator.GetDefibProgressData().GetPercentComplete(ACE_Medical_Defibrillation_EDefibProgressCategory.Charge) >= 1.0)
			return true;
		
		return false;
	}
}