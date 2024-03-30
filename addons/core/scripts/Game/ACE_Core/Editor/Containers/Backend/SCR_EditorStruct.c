//------------------------------------------------------------------------------------------------
//! Class which carries saved data for the editor.
//! Managed by SCR_DSSessionCallback.
[BaseContainerProps()]
modded class SCR_EditorStruct : SCR_JsonApiStruct
{
	// SCR_JsonApiStruct does not support array of PoD, hence we use ACE_VectorStruct as wrapper
	protected ref array<ref ACE_VectorStruct> m_aACE_DeletedEntityPositions = {};
	
	//------------------------------------------------------------------------------------------------
	void SCR_EditorStruct()
	{
		RegV("m_aACE_DeletedEntityPositions");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Write world data into the struct.
	override bool Serialize()
	{
		if (!super.Serialize())
			return false;
		
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!gameMode)
			return false;
		
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
		if (!super.Deserialize())
			return false;
		
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
		super.ClearCache();
		m_aACE_DeletedEntityPositions.Clear();
	}
}
