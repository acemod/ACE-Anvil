class ACE_Medical_Defibrillation_AnalysingState : ACE_Medical_Defibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_Defibrillation_DefibContext context)
	{
		super.OnEnter(context);
		
		context.m_pDefibrillator.SetDefibStateID(ACE_Medical_Defibrillation_EDefibStateID.ANALYSING);
		
		context.m_pDefibrillator.GetDefibProgressData().ResetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.Analysis);
		
		// Play analysing sound
		ACE_Medical_Defibrillation_DefibSoundManagerComponent manager = ACE_Medical_Defibrillation_ComponentManager.GetDefibSoundManagerComponent(
			context.m_pDefibrillator.GetOwner()
		);
		if (!manager)
			return;
		
		manager.PlaySoundGlobal(ACE_Medical_Defibrillation_SharedSounds.SOUNDANALYSING);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		
		// Update analysis timer
		context.m_pDefibrillator.GetDefibProgressData().ModifyTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.Analysis, timeSlice);
	}
}