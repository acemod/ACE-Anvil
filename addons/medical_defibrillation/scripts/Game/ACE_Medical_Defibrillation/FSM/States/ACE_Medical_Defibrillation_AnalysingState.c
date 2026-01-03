class ACE_Medical_Defibrillation_AnalysingState : ACE_Medical_Defibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_Defibrillation_DefibContext context)
	{
		super.OnEnter(context);
		
		context.m_pDefibrillator.GetDefibProgressData().ResetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.Analysis);
		
		// Play analysing sound
		context.m_pDefibrillator.GetSoundComponent().PlaySound(ACE_Medical_Defibrillation_DefibSounds.SOUNDANALYSING);
		
		Print("ACE_Medical_Defibrillation_AnalysingState::OnEnter | State entered: analysing");
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		
		// Update analysis timer
		context.m_pDefibrillator.GetDefibProgressData().ModifyTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.Analysis, timeSlice);
	}
}