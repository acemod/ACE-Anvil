class ACE_Medical_Defibrillation_ChargedState : ACE_Medical_Defibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_Defibrillation_DefibContext context)
	{
		super.OnEnter(context);
		
		context.m_pDefibrillator.SetDefibStateID(ACE_Medical_Defibrillation_EDefibStateID.CHARGED);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnExit(ACE_Medical_Defibrillation_DefibContext context)
	{
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
	}
}