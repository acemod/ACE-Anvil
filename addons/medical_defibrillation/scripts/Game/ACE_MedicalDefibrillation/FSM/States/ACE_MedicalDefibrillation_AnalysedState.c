//------------------------------------------------------------------------------------------------
class ACE_MedicalDefibrillation_AnalysedState : ACE_MedicalDefibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_MedicalDefibrillation_DefibrillatorContext context)
	{
		super.OnEnter(context);
		
		// Play shock advised/not advised sound effect
		
		Print("ACE_MedicalDefibrillation_AnalysedState::OnEnter | State entered: analysed");
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_MedicalDefibrillation_DefibrillatorContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
	}
}