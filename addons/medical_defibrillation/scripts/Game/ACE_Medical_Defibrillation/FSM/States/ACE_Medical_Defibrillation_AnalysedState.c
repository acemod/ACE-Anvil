//------------------------------------------------------------------------------------------------
class ACE_Medical_Defibrillation_AnalysedState : ACE_Medical_Defibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_Defibrillation_DefibContext context)
	{
		super.OnEnter(context);
			
		Print("ACE_Medical_Defibrillation_AnalysedState::OnEnter | State entered: analysed");
		
		// Play shock advised/not advised sound and move to charging
		if (context.m_pDefibrillator.IsShockableRhythm())
		{
			context.m_pDefibrillator.GetSoundComponent().PlaySound(ACE_Medical_Defibrillation_DefibSounds.SOUNDSHOCKADVISED)
			// Will transition to charging after 2.0 seconds in this state automatically. See: ACE_Medical_Defibrillation_ChargingStateTransition
		}
		else
		{
			// Not shockable, place on cooldowna and go back to connected (no transition)
			context.m_pDefibrillator.GetSoundComponent().PlaySound(ACE_Medical_Defibrillation_DefibSounds.SOUNDNOSHOCKADVISED);
			
			float cprTimerDuration = context.m_pDefibrillator.GetDefibProgressData().GetDuration(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown);
			context.m_pDefibrillator.GetDefibProgressData().SetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown, cprTimerDuration);
			
			context.m_pDefibrillator.SetDefibStateID(ACE_Medical_Defibrillation_EDefibStateID.CONNECTED);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
	}
}