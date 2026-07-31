//------------------------------------------------------------------------------------------------
class ACE_Medical_StartResuscitationTransition : ACE_FSM_ITransition<ACE_Medical_CharacterContext>
{
	//------------------------------------------------------------------------------------------------
	override bool ShouldBePerformed(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return context.m_pVitals.IsCPRPerformed();
	}
}
