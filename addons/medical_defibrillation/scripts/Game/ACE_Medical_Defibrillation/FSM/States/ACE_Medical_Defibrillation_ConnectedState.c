class ACE_Medical_Defibrillation_ConnectedState : ACE_Medical_Defibrillation_IDefibState
{
	float m_fLastCPRPaceTimer;
	
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_Defibrillation_DefibContext context)
	{
		super.OnEnter(context);
		
		m_fLastCPRPaceTimer = -1000; // Have a delay on the first one so that previous sound can end
		
		Print("ACE_Medical_Defibrillation_ConnectedState::OnEnter | State entered: connected");
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		
		if (m_fLastCPRPaceTimer >= 500 && context.m_pDefibrillator.GetDefibProgressData().GetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown) > 0)
		{
			context.m_pDefibrillator.GetSoundComponent().PlayDefibSoundServer(ACE_Medical_Defibrillation_DefibSounds.SOUNDCPRBEEP);
			m_fLastCPRPaceTimer = 0;
		}
		m_fLastCPRPaceTimer += timeSlice;
	}
}