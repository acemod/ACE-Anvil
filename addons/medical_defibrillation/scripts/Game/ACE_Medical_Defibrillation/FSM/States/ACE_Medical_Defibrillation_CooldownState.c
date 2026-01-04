class ACE_Medical_Defibrillation_CooldownState : ACE_Medical_Defibrillation_IDefibState
{
	float m_fLastCPRReminder = 0;
	
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_Defibrillation_DefibContext context)
	{
		super.OnEnter(context);
		
		Print("ACE_Medical_Defibrillation_CooldownState::OnEnter | State entered: cooldown");
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
	}
}