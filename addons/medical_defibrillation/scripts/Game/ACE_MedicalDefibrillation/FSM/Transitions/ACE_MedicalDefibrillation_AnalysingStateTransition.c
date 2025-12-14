/*
	Previous State: Connected
	Next State: Analysed

	Conditions:
	- Must have a patient connected
	- Must not have a shock cooldown
	- Must not be performing CPR
	- Be in connected state for at least 2 seconds

	- Defibrillator Emulation Notes
		- Manual: Cannot use this state
*/

class ACE_MedicalDefibrillation_AnalysingStateTransition : ACE_FSM_ITransition<ACE_MedicalDefibrillation_DefibrillatorContext>
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
		Print("ACE_MedicalDefibrillation_AnalysingStateTransition::OnPerform | Entering state: Analysing");
		// Play analysing sound
	}
	
	//------------------------------------------------------------------------------------------------
	override bool ShouldBePerformed(ACE_MedicalDefibrillation_DefibrillatorContext context, float timeSlice)
	{
		// Manual defib can't use this state
		if (context.m_pDefibrillator.GetDefibrillatorEmulation() == ACE_MedicalDefibrillation_EDefibrillatorEmulation.Manual_NotImplimented)
			return false;
		
		// Patient connected && no CPR is performed
		IEntity patient = context.m_pDefibrillator.GetPatient();
		if (!patient)
			return false;
		ACE_Medical_VitalsComponent vitals = ACE_Medical_VitalsComponent.Cast(patient.FindComponent(ACE_Medical_VitalsComponent));
		if (vitals.IsCPRPerformed())
			return false;
		
		// Defib in Connected state for at least 2 seconds
		ACE_MedicalDefibrillation_DefibProgressData defibProgress = context.m_pDefibrillator.GetDefibProgressData();
		if (defibProgress.GetTimer(ACE_MedicalDefibrillation_EDefibProgressCategory.CPRCooldown) > 0)
			return false;
		if (defibProgress.GetTimer(ACE_MedicalDefibrillation_EDefibProgressCategory.StateTimeElapsed) < 2000)
			return false;
		
		return true;
	}
}