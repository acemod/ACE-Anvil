class ACE_MedicalDefibrillation_DisconnectedState : ACE_MedicalDefibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_MedicalDefibrillation_DefibrillatorContext context)
	{
		super.OnEnter(context);
		
		// Play disconnected sound effect
		
		Print("ACE_MedicalDefibrillation_DisconnectedState::OnEnter | State entered: disconnected");
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_MedicalDefibrillation_DefibrillatorContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
	}
}