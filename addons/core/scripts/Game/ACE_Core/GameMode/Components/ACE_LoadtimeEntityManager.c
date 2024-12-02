//------------------------------------------------------------------------------------------------
class ACE_LoadtimeEntityManagerClass : SCR_BaseGameModeComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Methods for manipulating unreplicated loadtime entities
class ACE_LoadtimeEntityManager : SCR_BaseGameModeComponent
{
	// EntityID can't be properly replicated, so we use ints instead
	// One EntityID (64 bit) consists of two ints (32 bit each)
	[RplProp(onRplName: "DeleteInitialEntities")]
	protected ref array<int> m_aDeletedEntitiesBits = {};
	
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
		for (int i = 0; i < m_aDeletedEntitiesBits.Count(); i += 2)
		{
			DeleteEntityByIdLocal(EntityID.FromInt(m_aDeletedEntitiesBits[i], m_aDeletedEntitiesBits[i + 1]));
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Deletes unreplicated entities by ID for all machines
	//! Can only be called on the server
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void DeleteEntitiesByIdGlobal(notnull array<EntityID> entityIDs)
	{
		array<int> newBits = {};
		newBits.Reserve(2 * entityIDs.Count());
		
		foreach (EntityID entityID : entityIDs)
		{
			newBits.InsertAll(ACE_EntityIdHelper.ToInt(entityID));
		}
		
		m_aDeletedEntitiesBits.Reserve(m_aDeletedEntitiesBits.Count() + newBits.Count());
		m_aDeletedEntitiesBits.InsertAll(newBits);
		
		Rpc(RpcDo_DeleteEntityByBitsBroadcast, newBits);
		RpcDo_DeleteEntityByBitsBroadcast(newBits);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_DeleteEntityByBitsBroadcast(array<int> newBits)
	{
		for (int i = 0; i < newBits.Count(); i += 2)
		{
			DeleteEntityByIdLocal(EntityID.FromInt(newBits[i], newBits[i + 1]))
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Deletes unreplicated entity by ID on a local machine
	void DeleteEntityByIdLocal(EntityID entityID)
	{
		IEntity entity = GetGame().GetWorld().FindEntityByID(entityID);
		if (entity)
			delete entity;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return all deleted unreplicated entity IDs
	//! Can only be called on the server
	array<EntityID> GetDeletedEntityIDs()
	{
		array<EntityID> entityIDs = {};
		entityIDs.Reserve(m_aDeletedEntitiesBits.Count() / 2);
		
		for (int i = 0; i < m_aDeletedEntitiesBits.Count(); i += 2)
		{
			entityIDs.Insert(EntityID.FromInt(m_aDeletedEntitiesBits[i], m_aDeletedEntitiesBits[i + 1]))
		}
		
		return entityIDs;
	}
}
