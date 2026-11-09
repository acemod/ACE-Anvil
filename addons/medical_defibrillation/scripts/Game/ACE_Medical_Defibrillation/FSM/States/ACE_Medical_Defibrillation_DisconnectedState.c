class ACE_Medical_Defibrillation_DisconnectedState : ACE_Medical_Defibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_Defibrillation_DefibContext context)
	{
		super.OnEnter(context);
		
		context.m_pDefibrillator.SetDefibStateID(ACE_Medical_Defibrillation_EDefibStateID.DISCONNECTED);
		
		// Play disconnected sound effect
		context.m_pDefibrillator.SetPatient(null);
		context.m_pDefibrillator.GetDefibProgressData().ResetAll();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
	}
}