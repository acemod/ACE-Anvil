class ACE_MedicalDefibrillation_ChargedState : ACE_MedicalDefibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_MedicalDefibrillation_DefibrillatorContext context)
	{
		super.OnEnter(context);
		
		// Play charged sound effect
		
		Print("ACE_MedicalDefibrillation_ChargedState::OnEnter | State entered: charged");
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_MedicalDefibrillation_DefibrillatorContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
	}
}