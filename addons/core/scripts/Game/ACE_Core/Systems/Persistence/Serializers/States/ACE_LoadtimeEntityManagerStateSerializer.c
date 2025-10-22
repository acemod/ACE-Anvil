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
		
		context.WriteValue("version", 1);
		context.WriteValue("entityIDs", manager.GetDeletedEntityIDs());
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

		array<EntityID> entityIDs;
		context.Read(entityIDs);
		manager.DeleteEntitiesByIdGlobal(entityIDs);
		return true;
	}
}
