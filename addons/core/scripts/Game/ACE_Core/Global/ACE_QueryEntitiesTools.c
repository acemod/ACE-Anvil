//------------------------------------------------------------------------------------------------
class ACE_QueryEntitiesTools
{
	protected static ref array<IEntity> m_aEntities;
	
	//------------------------------------------------------------------------------------------------
	//! Return all entities inside an oriented bounding box
	static array<IEntity> GetAllEntitiesInOBB(vector mins, vector maxs, vector transform[4])
	{
		m_aEntities = {};
		GetGame().GetWorld().QueryEntitiesByOBB(mins, maxs, transform, QueryCallback);
		return m_aEntities;
	}
	
	//------------------------------------------------------------------------------------------------
	static protected bool QueryCallback(IEntity entity)
	{
		m_aEntities.Insert(entity);
		return true;
	}
}
