/*
	Previous State: Charging
	Next State: Charged

	Conditions:
	- Must have a patient connected
	- Must have charge percentage >= 1
*/

class ACE_MedicalDefibrillation_ChargedStateTransition : ACE_FSM_ITransition<ACE_MedicalDefibrillation_DefibrillatorContext>
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
		Print("ACE_MedicalDefibrillation_ChargedStateTransition::OnPerform | Entering state: Charged");
	}
	
	//------------------------------------------------------------------------------------------------
	override bool ShouldBePerformed(ACE_MedicalDefibrillation_DefibrillatorContext context, float timeSlice)
	{
		if (context.m_pDefibrillator.GetDefibrillatorEmulation() == ACE_MedicalDefibrillation_EDefibrillatorEmulation.Manual_NotImplimented)
			return false;
		
		IEntity patient = context.m_pDefibrillator.GetPatient();
		if (!patient)
			return false;
		
		if (context.m_pDefibrillator.GetDefibProgressData().GetPercentComplete(ACE_MedicalDefibrillation_EDefibProgressCategory.Charge) >= 1.0)
			return true;
		
		return false;
	}
}