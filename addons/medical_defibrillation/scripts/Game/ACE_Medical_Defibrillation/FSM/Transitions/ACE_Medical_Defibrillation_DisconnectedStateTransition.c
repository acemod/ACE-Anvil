class ACE_Medical_Defibrillation_DisconnectedStateTransition : ACE_FSM_ITransition<ACE_Medical_Defibrillation_DefibContext>
{
	//------------------------------------------------------------------------------------------------
	override void OnPerform(ACE_Medical_Defibrillation_DefibContext context)
	{	
		ACE_Medical_Defibrillation_DefibSoundManagerComponent manager = ACE_Medical_Defibrillation_ComponentManager.GetDefibSoundManagerComponent(
			context.m_pDefibrillator.GetOwner()
		);
		if (!manager)
			return;
		
		manager.TerminateAllSoundsGlobal();
		manager.PlaySoundGlobal(ACE_Medical_Defibrillation_SharedSounds.SOUNDDISCONNECTED);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool ShouldBePerformed(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		IEntity patient = context.m_pDefibrillator.GetPatient();
		if (!patient || !context.m_pDefibrillator.DefibrillatorPatientInRange())
			return true;
		
		return false;
	}
}