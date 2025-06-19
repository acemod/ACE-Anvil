//------------------------------------------------------------------------------------------------
class ACE_BaseSystem_EntityContext : Managed
{
	IEntity m_pEntity;
	float m_fLastUpdateTime;
	
	//------------------------------------------------------------------------------------------------
	void ACE_BaseSystem_EntityContext(IEntity entity)
	{
		m_pEntity = entity;
		m_fLastUpdateTime = GetGame().GetWorld().GetWorldTime();
	}
}

