//------------------------------------------------------------------------------------------------
class ACE_LoadtimeEntityManagerClass : SCR_BaseGameModeComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Methods for manipulating unreplicated loadtime entities
class ACE_LoadtimeEntityManager : SCR_BaseGameModeComponent
{
	[RplProp(onRplName: "DeleteInitialEntities")]
	protected ref array<EntityID> m_aDeletedEntities = {};
	
	protected static ACE_LoadtimeEntityManager s_pInstance;
	
	//------------------------------------------------------------------------------------------------
	void ACE_LoadtimeEntityManager(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		s_pInstance = this;
	}
	
	//------------------------------------------------------------------------------------------------
	static ACE_LoadtimeEntityManager GetInstance()
	{
		return s_pInstance;
	}
		
	//------------------------------------------------------------------------------------------------
	//! Ensures that already deleted unreplicated entities are deleted for JIPs
	void DeleteInitialEntities()
	{
		foreach (EntityID entityID : m_aDeletedEntities)
		{
			DeleteEntityByIdLocal(entityID);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Deletes unreplicated entity by ID for all machines
	//! Can only be called on the server
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void DeleteEntityByIdGlobal(EntityID entityID)
	{
		Rpc(DeleteEntityByIdLocal, entityID);
		DeleteEntityByIdLocal(entityID);
		m_aDeletedEntities.Insert(entityID);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Deletes unreplicated entities by position on a local machine
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void DeleteEntityByIdLocal(EntityID entityID)
	{
		IEntity entity = GetGame().GetWorld().FindEntityByID(entityID);
		if (entity)
			delete entity;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return all deleted unreplicated entity positions
	//! Can only be called on the server
	array<EntityID> GetDeletedEntities()
	{
		return m_aDeletedEntities;
	}
}
