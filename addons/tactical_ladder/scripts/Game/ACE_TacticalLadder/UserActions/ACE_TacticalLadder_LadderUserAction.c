//------------------------------------------------------------------------------------------------
class ACE_TacticalLadder_LadderUserAction : SCR_LadderUserAction
{
	protected ACE_TacticalLadderEntity m_pOwner;
	
	//------------------------------------------------------------------------------------------------
	protected override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		m_pOwner = ACE_TacticalLadderEntity.Cast(pOwnerEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	protected override bool CanBePerformedScript(IEntity user)
	{
		if (!m_pOwner)
			return false;
		
		// Update ladder idx
		m_iLadderComponentIndex = m_pOwner.GetCurrentLadderComponentIdx();
		if (m_iLadderComponentIndex < 0)
		{
			m_sCannotPerformReason = "#ACE_TacticalLadder-UserAction_TooShort";
			return false;
		}
		
		if (m_pOwner.IsTopExitObstructed())
		{
			m_sCannotPerformReason = "#ACE_TacticalLadder-UserAction_TopExitObstructed";
			return false;
		}
		
		return super.CanBePerformedScript(user);
	}
}
