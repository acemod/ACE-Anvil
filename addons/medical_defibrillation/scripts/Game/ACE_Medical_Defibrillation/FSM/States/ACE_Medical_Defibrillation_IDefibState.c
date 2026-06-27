class ACE_Medical_Defibrillation_IDefibState : ACE_FSM_IState<ACE_Medical_Defibrillation_DefibContext>
{
	protected ACE_Medical_CharacterContext m_pPatient;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_Defibrillation_IDefibState(ACE_FSM_EStateID id)
	{
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_Defibrillation_DefibContext context)
	{
		super.OnEnter(context);
		
		context.m_pDefibrillator.GetDefibProgressData().ResetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.StateTimeElapsed);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		
		// Update total time in state
		context.m_pDefibrillator.GetDefibProgressData().ModifyTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.StateTimeElapsed, timeSlice, replicate : false);
	}
}