class ACE_Medical_Defibrillation_ConnectedState : ACE_Medical_Defibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_Defibrillation_DefibContext context)
	{
		super.OnEnter(context);
		
		context.m_pDefibrillator.m_pSounds.m_fContinueCPRTimer = 10000 - 2000;
		context.m_pDefibrillator.m_pSounds.m_fLastCPRPaceTimer = -2000; // Have a delay on the first one so that previous sound can end
		
		Print("ACE_Medical_Defibrillation_ConnectedState::OnEnter | State entered: connected");
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		
		// Reduce CPR cooldown
		context.m_pDefibrillator.GetDefibProgressData().ModifyTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown, -timeSlice);
		
		if (context.m_pDefibrillator.GetDefibProgressData().GetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown) > 0)
		{
			// Remind players to do CPR
			if (context.m_pDefibrillator.m_pSounds.m_fContinueCPRTimer >= 10000)
			{
				context.m_pDefibrillator.GetSoundComponent().PlaySound(ACE_Medical_Defibrillation_DefibSounds.SOUNDCPRREMINDER);
				context.m_pDefibrillator.m_pSounds.m_fContinueCPRTimer = 0;
			}
			
			// Play CPR pacing beats
			if (context.m_pDefibrillator.m_pSounds.m_fLastCPRPaceTimer >= 500)
			{
				context.m_pDefibrillator.GetSoundComponent().PlaySound(ACE_Medical_Defibrillation_DefibSounds.SOUNDCPRBEEP);
				context.m_pDefibrillator.m_pSounds.m_fLastCPRPaceTimer = 0;
			}
		}

		context.m_pDefibrillator.m_pSounds.m_fContinueCPRTimer += timeSlice;
		context.m_pDefibrillator.m_pSounds.m_fLastCPRPaceTimer += timeSlice;
	}
}