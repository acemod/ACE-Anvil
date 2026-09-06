class ACE_Medical_Defibrillation_ChargedState : ACE_Medical_Defibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_Defibrillation_DefibContext context)
	{
		super.OnEnter(context);
		
		context.m_pDefibrillator.SetDefibStateID(ACE_Medical_Defibrillation_EDefibStateID.CHARGED);
		
		// Play sounds
		ACE_Medical_Defibrillation_DefibSoundManagerComponent manager = ACE_Medical_Defibrillation_ComponentManager.GetDefibSoundManagerComponent(
			context.m_pDefibrillator.GetOwner()
		);
		if (!manager)
			return;
		
		manager.m_pSoundTimers.m_fChargedBeepTimer = 0;
		manager.m_pSoundTimers.m_iChargedBeepPhase = 0;
		
		manager.SetChargedBeepLoop(true);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnExit(ACE_Medical_Defibrillation_DefibContext context)
	{
		ACE_Medical_Defibrillation_DefibSoundManagerComponent manager = ACE_Medical_Defibrillation_ComponentManager.GetDefibSoundManagerComponent(
			context.m_pDefibrillator.GetOwner()
		);
		if (!manager)
			return;
		
		manager.SetChargedBeepLoop(false);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
	}
}