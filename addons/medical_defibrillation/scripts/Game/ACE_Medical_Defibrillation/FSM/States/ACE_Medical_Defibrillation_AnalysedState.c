//------------------------------------------------------------------------------------------------
class ACE_Medical_Defibrillation_AnalysedState : ACE_Medical_Defibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_Defibrillation_DefibContext context)
	{
		super.OnEnter(context);
		
		context.m_pDefibrillator.SetDefibStateID(ACE_Medical_Defibrillation_EDefibStateID.ANALYSED);
		
		ACE_Medical_Defibrillation_DefibSoundManagerComponent manager = ACE_Medical_Defibrillation_ComponentManager.GetDefibSoundManagerComponent(
			context.m_pDefibrillator.GetOwner()
		);
		if (manager)
		{
			// Play shock advised/not advised sound and move to charging
			if (context.m_pDefibrillator.IsShockableRhythm() || context.m_pDefibrillator.GetDebugAlwaysShockableRhythm())
			{
				manager.PlaySoundGlobal(ACE_Medical_Defibrillation_SharedSounds.SOUNDSHOCKADVISED);
				// Will transition to charging after 2.0 seconds in this state automatically. See: ACE_Medical_Defibrillation_ChargingStateTransition
			}
			else
			{
				// Not shockable, place on cooldowna and go back to connected (no transition)
				manager.PlaySoundGlobal(ACE_Medical_Defibrillation_SharedSounds.SOUNDNOSHOCKADVISED);
				
				float cprTimerDuration = context.m_pDefibrillator.GetDefibProgressData().GetDuration(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown);
				context.m_pDefibrillator.GetDefibProgressData().SetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown, cprTimerDuration);
				
				context.m_pDefibrillator.SetDefibStateID(ACE_Medical_Defibrillation_EDefibStateID.CONNECTED);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
	}
}