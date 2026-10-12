class ACE_Medical_Defibrillation_ChargingState : ACE_Medical_Defibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_Defibrillation_DefibContext context)
	{
		super.OnEnter(context);
		
		context.m_pDefibrillator.SetDefibStateID(ACE_Medical_Defibrillation_EDefibStateID.CHARGING);
		
		context.m_pDefibrillator.GetDefibProgressData().ResetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.Charge);
		
		// Play analysing sound effect
		ACE_Medical_Defibrillation_DefibSoundManagerComponent manager = ACE_Medical_Defibrillation_ComponentManager.GetDefibSoundManagerComponent(
			context.m_pDefibrillator.GetOwner()
		);
		if (!manager)
			return;
		
		manager.PlaySoundGlobal(ACE_Medical_Defibrillation_SharedSounds.SOUNDCHARGING);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		
		// Update charge timer
		context.m_pDefibrillator.GetDefibProgressData().ModifyTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.Charge, timeSlice);
	}
}