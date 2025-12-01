//------------------------------------------------------------------------------------------------
//! Methods for manipulating unreplicated loadtime entities
class ACE_LoadtimeEntityManager : GameSystem
{
	// array<EntityID> can't be properly replicated (see https://feedback.bistudio.com/T186903) so we use a wrapper instead
	[RplProp(onRplName: "DeleteInitialEntities")]
	protected ref array<ref ACE_EntityIdWrapper> m_aDeletedEntityIDs = {};
	
	//------------------------------------------------------------------------------------------------
	static ACE_LoadtimeEntityManager GetInstance()
	{
		ChimeraWorld world = GetGame().GetWorld();
		return ACE_LoadtimeEntityManager.Cast(world.FindSystem(ACE_LoadtimeEntityManager));
	}
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(false)
			.SetUnique(true)
			.SetLocation(WorldSystemLocation.Both)
			.AddPoint(WorldSystemPoint.RuntimeStarted)
	}
		
	//------------------------------------------------------------------------------------------------
	//! Ensures that already deleted unreplicated entities are deleted for JIPs
	void DeleteInitialEntities()
	{
		foreach (ACE_EntityIdWrapper idWrapper : m_aDeletedEntityIDs)
		{
			DeleteEntityByIdLocal(idWrapper.GetID());
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Deletes unreplicated entities by ID for all machines
	//! Can only be called on the server
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void DeleteEntitiesByIdGlobal(notnull array<EntityID> entityIDs)
	{
		array<ref ACE_EntityIdWrapper> newIDs = {};
		newIDs.Reserve(entityIDs.Count());
		m_aDeletedEntityIDs.Reserve(m_aDeletedEntityIDs.Count() + newIDs.Count());
		
		foreach (EntityID entityID : entityIDs)
		{
			ACE_EntityIdWrapper idWrapper = ACE_EntityIdWrapper.CreateID(entityID);
			newIDs.Insert(idWrapper);
			m_aDeletedEntityIDs.Insert(idWrapper);
		}
				
		Rpc(RpcDo_DeleteEntityByBitsBroadcast, newIDs);
		RpcDo_DeleteEntityByBitsBroadcast(newIDs);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_DeleteEntityByBitsBroadcast(array<ref ACE_EntityIdWrapper> newIDs)
	{
		foreach (ACE_EntityIdWrapper idWrapper : newIDs)
		{
			DeleteEntityByIdLocal(idWrapper.GetID());
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
		entityIDs.Reserve(m_aDeletedEntityIDs.Count());
		
		foreach (ACE_EntityIdWrapper idWrapper : m_aDeletedEntityIDs)
		{
			entityIDs.Insert(idWrapper.GetID());
		}
		
		return entityIDs;
	}
}

//------------------------------------------------------------------------------------------------
//! Temporary workaround for https://feedback.bistudio.com/T186903
class ACE_EntityIdWrapper : Managed
{
	static const int SNAPSHOT_SIZE_VALUE = 8; //--- EntityID is 64 bit integer
	protected EntityID m_iID;
	
	//------------------------------------------------------------------------------------------------
	static ACE_EntityIdWrapper CreateID(EntityID id)
	{
		ACE_EntityIdWrapper instance = new ACE_EntityIdWrapper();
		instance.m_iID = id;
		return instance;
	}
	
	//------------------------------------------------------------------------------------------------
	EntityID GetID()
	{
		return m_iID;
	}
	
	//------------------------------------------------------------------------------------------------
	static void Encode(SSnapSerializerBase snapshot, ScriptCtx hint, ScriptBitSerializer packet) 
	{
		snapshot.Serialize(packet, SNAPSHOT_SIZE_VALUE);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Decode(ScriptBitSerializer packet, ScriptCtx hint, SSnapSerializerBase snapshot) 
	{
		return snapshot.Serialize(packet, SNAPSHOT_SIZE_VALUE);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx hint) 
	{
		return lhs.CompareSnapshots(rhs, SNAPSHOT_SIZE_VALUE);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool PropCompare(ACE_EntityIdWrapper prop, SSnapSerializerBase snapshot, ScriptCtx hint) 
	{
		return snapshot.Compare(prop.m_iID, SNAPSHOT_SIZE_VALUE);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Extract(ACE_EntityIdWrapper prop, ScriptCtx hint, SSnapSerializerBase snapshot) 
	{
		snapshot.SerializeBytes(prop.m_iID, SNAPSHOT_SIZE_VALUE);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx hint, ACE_EntityIdWrapper prop) 
	{
		return Extract(prop, hint, snapshot);
	}
}
