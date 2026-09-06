//------------------------------------------------------------------------------------------------
class ACE_Medical_Defibrillation_AnalysedState : ACE_Medical_Defibrillation_IDefibState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_Defibrillation_DefibContext context)
	{
		super.OnEnter(context);
		
		context.m_pDefibrillator.SetDefibStateID(ACE_Medical_Defibrillation_EDefibStateID.ANALYSED);
		
		if (context.m_pDefibrillator.IsShockableRhythm() || context.m_pDefibrillator.GetDebugAlwaysShockableRhythm())
		{
			// Will transition to charging after 2.0 seconds in this state automatically. See: ACE_Medical_Defibrillation_ChargingStateTransition
		}
		else
		{
			float cprTimerDuration = context.m_pDefibrillator.GetDefibProgressData().GetDuration(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown);
			context.m_pDefibrillator.GetDefibProgressData().SetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown, cprTimerDuration);
			
			context.m_pDefibrillator.SetDefibStateID(ACE_Medical_Defibrillation_EDefibStateID.CONNECTED);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_Defibrillation_DefibContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
	}
}