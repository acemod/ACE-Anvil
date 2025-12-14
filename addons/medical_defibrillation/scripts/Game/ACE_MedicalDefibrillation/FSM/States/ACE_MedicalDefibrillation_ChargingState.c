class ACE_MedicalDefibrillation_ChargingState : ACE_MedicalDefibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_MedicalDefibrillation_DefibrillatorContext context)
	{
		super.OnEnter(context);
		
		context.m_pDefibrillator.GetDefibProgressData().ResetTimer(ACE_MedicalDefibrillation_EDefibProgressCategory.Charge);
		
		// Play analysing sound effect
		
		Print("ACE_MedicalDefibrillation_ChargingState::OnEnter | State entered: charging");
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_MedicalDefibrillation_DefibrillatorContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		
		// Update charge timer
		context.m_pDefibrillator.GetDefibProgressData().ModifyTimer(ACE_MedicalDefibrillation_EDefibProgressCategory.Charge, timeSlice);
	}
}