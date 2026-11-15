modded class ACE_Medical_StableState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_CharacterContext context)
	{
		super.OnEnter(context);
		
		context.m_pVitals.ModifyTimesArrested(-(context.m_pVitals.GetTimesArrested()));
		context.m_pVitals.ResetTimeOfArrest();
	}
}