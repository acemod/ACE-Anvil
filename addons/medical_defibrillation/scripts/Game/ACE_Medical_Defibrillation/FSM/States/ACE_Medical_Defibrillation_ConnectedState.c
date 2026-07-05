class ACE_Medical_Defibrillation_ConnectedState : ACE_Medical_Defibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_Defibrillation_DefibContext context)
	{
		super.OnEnter(context);
		
		context.m_pDefibrillator.SetDefibStateID(ACE_Medical_Defibrillation_EDefibStateID.CONNECTED);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		
		// Reduce CPR cooldown
		context.m_pDefibrillator.GetDefibProgressData().ModifyTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown, -timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnExit(ACE_Medical_Defibrillation_DefibContext context)
	{
	}
}