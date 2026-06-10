class ACE_Medical_Defibrillation_ConnectedState : ACE_Medical_Defibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_Defibrillation_DefibContext context)
	{
		super.OnEnter(context);
		
		context.m_pDefibrillator.SetDefibStateID(ACE_Medical_Defibrillation_EDefibStateID.CONNECTED);
		
		context.m_pDefibrillator.m_pSounds.m_fContinueCPRTimer = 10000 - 2000;
		context.m_pDefibrillator.m_pSounds.m_fLastCPRPaceTimer = -2000; // Have a delay on the first one so that previous sound can end
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		
		// Reduce CPR cooldown
		context.m_pDefibrillator.GetDefibProgressData().ModifyTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown, -timeSlice);
		
		if (context.m_pDefibrillator.GetDefibProgressData().GetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown) > 0)
		{
			context.m_pDefibrillator.m_pSounds.m_fContinueCPRTimer += timeSlice;
			
			// Remind players to do CPR
			if (context.m_pDefibrillator.m_pSounds.m_fContinueCPRTimer >= 10000)
			{
				context.m_pDefibrillator.PlaySound(ACE_Medical_Defibrillation_DefibSounds.SOUNDCPRREMINDER);
				context.m_pDefibrillator.m_pSounds.m_fContinueCPRTimer = 0;
			}
			
			// Temp Disable Until Better Networking - On remote clients it comes out to 600ms so there is some delay
			// Play CPR pacing beats
			context.m_pDefibrillator.SetCPRBeepLoop(true);
		}
		else
		{
			context.m_pDefibrillator.SetCPRBeepLoop(false);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnExit(ACE_Medical_Defibrillation_DefibContext context)
	{
		// st st st st stop the beats
		context.m_pDefibrillator.SetCPRBeepLoop(false);
	}
}