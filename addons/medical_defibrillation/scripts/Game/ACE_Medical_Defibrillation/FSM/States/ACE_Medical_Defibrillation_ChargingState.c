class ACE_Medical_Defibrillation_ChargingState : ACE_Medical_Defibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_Defibrillation_DefibContext context)
	{
		super.OnEnter(context);
		
		context.m_pDefibrillator.SetDefibStateID(ACE_Medical_Defibrillation_EDefibStateID.CHARGING);
		
		context.m_pDefibrillator.GetDefibProgressData().ResetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.Charge);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		
		// Update charge timer
		context.m_pDefibrillator.GetDefibProgressData().ModifyTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.Charge, timeSlice);
	}
}