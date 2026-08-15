//------------------------------------------------------------------------------------------------
modded class ACE_Medical_ReviveTransition : ACE_FSM_ITransition<ACE_Medical_CharacterContext>
{
	//------------------------------------------------------------------------------------------------
	//! No revive if SpO2 is too low
	override protected float ComputeReviveChance(ACE_Medical_CharacterContext context)
	{
		if (context.m_pVitals.GetSpO2() < s_pCirculationSettings.m_CardiacArrestThresholds.m_fSpO2)
			return 0;
		
		return super.ComputeReviveChance(context);
	}
}
