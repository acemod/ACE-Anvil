//------------------------------------------------------------------------------------------------
//! Add methods for deleting unreplicated entities
modded class SCR_BaseGameMode : BaseGameMode
{
	[RplProp(onRplName: "ACE_DeleteInitialEntityPositions")]
	protected ref array<vector> m_aACE_DeletedEntityPositions = {};

	//------------------------------------------------------------------------------------------------
	//! Ensures that already deleted unreplicated entities are deleted for JIPs
	void ACE_DeleteInitialEntityPositions()
	{
		ACE_DeleteEntitiesAtPositionsLocal(m_aACE_DeletedEntityPositions);
	}

	//------------------------------------------------------------------------------------------------
	//! Deletes unreplicated entities by position for all machines
	//! Can only be called on the server
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void ACE_DeleteEntitiesAtPositionsGlobal(array<vector> entityPositions)
	{
		m_aACE_DeletedEntityPositions.InsertAll(entityPositions);
		ACE_DeleteEntitiesAtPositionsLocal(entityPositions);
		Rpc(ACE_DeleteEntitiesAtPositionsLocal, entityPositions);
	}

	//------------------------------------------------------------------------------------------------
	//! Deletes unreplicated entities by position on a local machine
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void ACE_DeleteEntitiesAtPositionsLocal(array<vector> entityPositions)
	{
		ACE_QueryNearestEntity query = new ACE_QueryNearestEntity(0.01);

		foreach (vector pos: entityPositions)
		{
			IEntity entity = query.GetEntity(pos);
			if (entity)
				SCR_EntityHelper.DeleteEntityAndChildren(entity);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Return all deleted unreplicated entity positions
	//! Can only be called on the server
	array<vector> ACE_GetDeletedEntityPositions()
	{
		return m_aACE_DeletedEntityPositions;
	}
}
