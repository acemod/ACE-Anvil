class ACE_Medical_Defibrillation_DisconnectedStateTransition : ACE_FSM_ITransition<ACE_Medical_Defibrillation_DefibContext>
{
	//------------------------------------------------------------------------------------------------
	override void OnPerform(ACE_Medical_Defibrillation_DefibContext context)
	{		
		context.m_pDefibrillator.PlaySound(ACE_Medical_Defibrillation_DefibSounds.SOUNDDISCONNECTED);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool ShouldBePerformed(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		IEntity patient = context.m_pDefibrillator.GetPatient();
		if (!patient)
			return true;
		
		return false;
	}
}