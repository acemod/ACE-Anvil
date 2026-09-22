//------------------------------------------------------------------------------------------------
class ACE_ActionsHelperEntityClass : GenericEntityClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_ActionsHelperEntity : GenericEntity
{
	protected IEntity m_AssociatedEntity;
	
	//----------------------------------------------------------------------------------------
	void SetAssociatedEntity(IEntity entity)
	{
		m_AssociatedEntity = entity;
	}
	
	//----------------------------------------------------------------------------------------
	IEntity GetAssociatedEntity()
	{
		return m_AssociatedEntity;
	}
}
