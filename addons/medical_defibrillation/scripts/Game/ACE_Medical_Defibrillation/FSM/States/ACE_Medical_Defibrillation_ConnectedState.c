class ACE_Medical_Defibrillation_ConnectedState : ACE_Medical_Defibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_Defibrillation_DefibContext context)
	{
		super.OnEnter(context);
		
		context.m_pDefibrillator.SetDefibStateID(ACE_Medical_Defibrillation_EDefibStateID.CONNECTED);
		
		// Modify sound timers
		ACE_Medical_Defibrillation_DefibSoundManagerComponent manager = ACE_Medical_Defibrillation_ComponentManager.GetDefibSoundManagerComponent(
			context.m_pDefibrillator.GetOwner()
		);
		if (!manager)
			return;
		
		const float CPR_REMINDER_INTERVAL_MS = 10000;
		const float CPR_BEATS_START_DELAY_MS = 2000;
		
		manager.m_pSoundTimers.m_fContinueCPRTimer = CPR_REMINDER_INTERVAL_MS - CPR_BEATS_START_DELAY_MS;
		manager.m_pSoundTimers.m_fLastCPRPaceTimer = -CPR_BEATS_START_DELAY_MS;
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		
		// Reduce CPR cooldown
		context.m_pDefibrillator.GetDefibProgressData().ModifyTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown, -timeSlice);
		
		// Play sounds
		ACE_Medical_Defibrillation_DefibSoundManagerComponent manager = ACE_Medical_Defibrillation_ComponentManager.GetDefibSoundManagerComponent(
			context.m_pDefibrillator.GetOwner()
		);
		if (!manager)
			return;
		
		if (context.m_pDefibrillator.GetDefibProgressData().GetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown) > 0)
		{
			manager.m_pSoundTimers.m_fContinueCPRTimer += timeSlice;
			
			// Remind players to do CPR
			if (manager.m_pSoundTimers.m_fContinueCPRTimer >= 10000)
			{
				manager.PlaySoundGlobal(ACE_Medical_Defibrillation_SharedSounds.SOUNDCPRREMINDER);
				manager.m_pSoundTimers.m_fContinueCPRTimer = 0;
			}
			
			// Temp Disable Until Better Networking - On remote clients it comes out to 600ms so there is some delay
			// Play CPR pacing beats
			manager.SetCPRBeepLoop(true);
		}
		else
		{
			manager.SetCPRBeepLoop(false);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnExit(ACE_Medical_Defibrillation_DefibContext context)
	{
		// st st st st stop the beats
		ACE_Medical_Defibrillation_DefibSoundManagerComponent manager = ACE_Medical_Defibrillation_ComponentManager.GetDefibSoundManagerComponent(
			context.m_pDefibrillator.GetOwner()
		);
		if (!manager)
			return;
		
		manager.SetCPRBeepLoop(false);
	}
}