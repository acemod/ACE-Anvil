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
		
		m_iLadderComponentIndex = m_pOwner.GetCurrentLadderComponentIdx();
		if (m_iLadderComponentIndex < 0)
			return false;
		
		return super.CanBePerformedScript(user);
	}
}
