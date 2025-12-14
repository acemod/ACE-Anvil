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

class ACE_Medical_Defibrillation_AnalysingStateTransition : ACE_FSM_ITransition<ACE_Medical_Defibrillation_DefibContext>
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
		Print("ACE_Medical_Defibrillation_AnalysingStateTransition::OnPerform | Entering state: Analysing");
		context.m_pDefibrillator.GetSoundComponent().PlayDefibSoundServer(ACE_Medical_Defibrillation_DefibSounds.SOUNDANALYSING);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool ShouldBePerformed(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		// Manual defib can't use this state
		if (context.m_pDefibrillator.GetDefibrillatorEmulation() == ACE_Medical_Defibrillation_EDefibEmulation.Manual_NotImplimented)
			return false;
		
		// Patient connected && no CPR is performed
		IEntity patient = context.m_pDefibrillator.GetPatient();
		if (!patient)
			return false;
		ACE_Medical_VitalsComponent vitals = ACE_Medical_VitalsComponent.Cast(patient.FindComponent(ACE_Medical_VitalsComponent));
		if (vitals.IsCPRPerformed())
			return false;
		
		// Defib in Connected state for at least 2 seconds
		ACE_Medical_Defibrillation_DefibProgressData defibProgress = context.m_pDefibrillator.GetDefibProgressData();
		float cprCooldown = defibProgress.GetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown);
		if (cprCooldown > 0)
			return false;

		float timeInState = defibProgress.GetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.StateTimeElapsed);
		if (timeInState < 2000)
			return false;
		
		return true;
	}
}