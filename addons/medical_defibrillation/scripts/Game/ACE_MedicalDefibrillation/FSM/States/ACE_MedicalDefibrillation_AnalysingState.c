class ACE_MedicalDefibrillation_AnalysingState : ACE_MedicalDefibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_MedicalDefibrillation_DefibrillatorContext context)
	{
		super.OnEnter(context);
		
		context.m_pDefibrillator.GetDefibProgressData().ResetTimer(ACE_MedicalDefibrillation_EDefibProgressCategory.Analysis);
		
		// Play analysing sound effect
		
		Print("ACE_MedicalDefibrillation_AnalysingState::OnEnter | State entered: analysing");
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_MedicalDefibrillation_DefibrillatorContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		
		// Update analysis timer
		context.m_pDefibrillator.GetDefibProgressData().ModifyTimer(ACE_MedicalDefibrillation_EDefibProgressCategory.Analysis, timeSlice);
	}
}