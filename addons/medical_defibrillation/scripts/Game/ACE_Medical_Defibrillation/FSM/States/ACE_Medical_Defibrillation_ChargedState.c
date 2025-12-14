class ACE_Medical_Defibrillation_ChargedState : ACE_Medical_Defibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_Defibrillation_DefibContext context)
	{
		super.OnEnter(context);
		
		// Play charged sound effect
		
		Print("ACE_Medical_Defibrillation_ChargedState::OnEnter | State entered: charged");
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
	}
}