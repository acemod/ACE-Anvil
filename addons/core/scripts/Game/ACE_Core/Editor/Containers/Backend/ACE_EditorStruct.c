//------------------------------------------------------------------------------------------------
//! Class which carries saved data for the editor.
//! Managed by SCR_DSSessionCallback.
class ACE_EditorStruct : SCR_JsonApiStruct
{
	protected ref array<string> m_aACE_DeletedEntityIDs = {};
	
	//------------------------------------------------------------------------------------------------
	void ACE_EditorStruct()
	{
		RegV("m_aACE_DeletedEntityIDs");
	}
	
	//------------------------------------------------------------------------------------------------
	// Print out contents of saved data.
	override void Log()
	{
		Print("--- ACE_EditorStruct ------------------------");
		for (int i = 0, count = m_aACE_DeletedEntityIDs.Count(); i < count; i++)
		{
			Print("Removed entity with ID: " + m_aACE_DeletedEntityIDs[i]);
		}
		Print("---------------------------------------------");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Write world data into the struct.
	override bool Serialize()
	{
		ACE_LoadtimeEntityManager manager = ACE_LoadtimeEntityManager.GetInstance();
		if (!manager)
			return false;
		
		array<EntityID> entityIDs = manager.GetDeletedEntityIDs();
		m_aACE_DeletedEntityIDs.Clear();
		m_aACE_DeletedEntityIDs.Reserve(entityIDs.Count());
		
		foreach (EntityID entityID : entityIDs)
		{
			m_aACE_DeletedEntityIDs.Insert(ACE_EntityIdHelper.ToString(entityID));
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Read data from the struct and apply them to the world.
	override bool Deserialize()
	{	
		ACE_LoadtimeEntityManager manager = ACE_LoadtimeEntityManager.GetInstance();
		if (!manager)
			return false;
		
		array<EntityID> entityIDs = {};
		entityIDs.Reserve(m_aACE_DeletedEntityIDs.Count());
		
		foreach (string str : m_aACE_DeletedEntityIDs)
		{
			EntityID entity = ACE_EntityIdHelper.FromString(str);
			if (entity != EntityID.INVALID)
				entityIDs.Insert(entity);
		}
		
		manager.DeleteEntitiesByIdGlobal(entityIDs);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Clear cached data.
	override void ClearCache()
	{
		m_aACE_DeletedEntityIDs.Clear();
	}
}
