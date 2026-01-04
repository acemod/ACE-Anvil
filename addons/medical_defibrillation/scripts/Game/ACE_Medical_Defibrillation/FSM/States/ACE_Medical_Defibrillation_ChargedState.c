class ACE_Medical_Defibrillation_ChargedState : ACE_Medical_Defibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_Defibrillation_DefibContext context)
	{
		super.OnEnter(context);
		
		context.m_pDefibrillator.m_pSounds.m_fChargedBeepTimer = 0;
		context.m_pDefibrillator.m_pSounds.m_iChargedBeepPhase = 0;
		
		Print("ACE_Medical_Defibrillation_ChargedState::OnEnter | State entered: charged");
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		
		const float BEEP_INTERVAL = 250;
		
		if (context.m_pDefibrillator.m_pSounds.m_fChargedBeepTimer >= BEEP_INTERVAL)
		{
			if (Math.Mod(context.m_pDefibrillator.m_pSounds.m_iChargedBeepPhase, 2) == 0)
			{
				context.m_pDefibrillator.GetSoundComponent().PlaySound(ACE_Medical_Defibrillation_DefibSounds.SOUNDCHARGEDBEEPLOW);
			}
			else
			{
				context.m_pDefibrillator.GetSoundComponent().PlaySound(ACE_Medical_Defibrillation_DefibSounds.SOUNDCHARGEDBEEPHIGH);
			}
			
			// Advance to next phase and reset timer
			context.m_pDefibrillator.m_pSounds.m_iChargedBeepPhase++;
			context.m_pDefibrillator.m_pSounds.m_fChargedBeepTimer = 0;
		}
		context.m_pDefibrillator.m_pSounds.m_fChargedBeepTimer += timeSlice;
	}
}