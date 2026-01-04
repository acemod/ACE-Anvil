class ACE_Medical_Defibrillation_DisconnectedState : ACE_Medical_Defibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_Defibrillation_DefibContext context)
	{
		super.OnEnter(context);
		
		// Play disconnected sound effect
		Print("ACE_Medical_Defibrillation_DisconnectedState::OnEnter | State entered: disconnected");
		context.m_pDefibrillator.SetPatient(null);
		context.m_pDefibrillator.GetDefibProgressData().ResetAll();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
	}
}