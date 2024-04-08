//------------------------------------------------------------------------------------------------
//! Class which carries saved data for the editor.
//! Managed by SCR_DSSessionCallback.
class ACE_EditorStruct : SCR_JsonApiStruct
{
	// SCR_JsonApiStruct does not support array of PoD, hence we use ACE_VectorStruct as wrapper
	protected ref array<ref ACE_VectorStruct> m_aACE_DeletedEntityPositions = {};
	
	//------------------------------------------------------------------------------------------------
	void ACE_EditorStruct()
	{
		RegV("m_aACE_DeletedEntityPositions");
	}
	
	//------------------------------------------------------------------------------------------------
	// Print out contents of saved data.
	override void Log()
	{
		Print("--- ACE_EditorStruct ------------------------");
		for (int i = 0, count = m_aACE_DeletedEntityPositions.Count(); i < count; i++)
		{
			Print("Removed entity position: " + m_aACE_DeletedEntityPositions[i]);
		}
		Print("---------------------------------------------");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Write world data into the struct.
	override bool Serialize()
	{
		
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!gameMode)
			return false;
		
		m_aACE_DeletedEntityPositions.Clear();
		
		foreach (vector pos : gameMode.ACE_GetDeletedEntityPositions())
		{
			m_aACE_DeletedEntityPositions.Insert(new ACE_VectorStruct(pos));
		}

		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Read data from the struct and apply them in the world.
	override bool Deserialize()
	{	
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!gameMode)
			return false;
		
		array<vector> deletedEntityPositions = {};
		
		foreach (ACE_VectorStruct posStruct : m_aACE_DeletedEntityPositions)
		{
			deletedEntityPositions.Insert(posStruct.GetVector());
		};
		
		gameMode.ACE_DeleteEntitiesAtPositionsGlobal(deletedEntityPositions);
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Clear cached data.
	override void ClearCache()
	{
		m_aACE_DeletedEntityPositions.Clear();
	}
}
