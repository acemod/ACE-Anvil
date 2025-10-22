//------------------------------------------------------------------------------------------------
class ACE_LoadtimeEntityManagerState : PersistentState
{
}

//------------------------------------------------------------------------------------------------
class ACE_LoadtimeEntityManagerStateSerializer : ScriptedStateSerializer
{
	//------------------------------------------------------------------------------------------------
	override static typename GetTargetType()
	{
		return ACE_LoadtimeEntityManagerState;
	}
	
	//------------------------------------------------------------------------------------------------
	override ESerializeResult Serialize(notnull Managed instance, notnull BaseSerializationSaveContext context)
	{
		ACE_LoadtimeEntityManager manager = ACE_LoadtimeEntityManager.GetInstance();
		if (!manager)
			return ESerializeResult.DEFAULT;
		
		array<EntityID> deletedEntityIDs = manager.GetDeletedEntityIDs();
		if (deletedEntityIDs.IsEmpty())
			return ESerializeResult.DEFAULT;
		
		context.WriteValue("version", 1);
		
		array<string> stringifiedIDs = {};
		stringifiedIDs.Reserve(deletedEntityIDs.Count());
		
		foreach (EntityID entityID : deletedEntityIDs)
		{
			stringifiedIDs.Insert(ACE_EntityIdHelper.ToString(entityID));
		}
		
		context.WriteValue("deletedEntityIDs", stringifiedIDs);
		return ESerializeResult.OK;
	}

	//------------------------------------------------------------------------------------------------
	override bool Deserialize(notnull Managed instance, notnull BaseSerializationLoadContext context)
	{
		ACE_LoadtimeEntityManager manager = ACE_LoadtimeEntityManager.GetInstance();
		if (!manager)
			return false;
		
		int version;
		context.Read(version);

		array<string> stringifiedIDs;
		context.Read(stringifiedIDs);
		array<EntityID> deletedEntityIDs = {};
		deletedEntityIDs.Reserve(stringifiedIDs.Count());

		foreach (string str : stringifiedIDs)
		{
			EntityID entityID = ACE_EntityIdHelper.FromString(str);
			if (entityID != EntityID.INVALID)
				deletedEntityIDs.Insert(entityID);
		}
		
		manager.DeleteEntitiesByIdGlobal(deletedEntityIDs);
		return true;
	}
}
