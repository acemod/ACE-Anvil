//------------------------------------------------------------------------------------------------
//! Add request for spawning a buildable prefab
//! Attached to the player controller
modded class SCR_CampaignBuildingNetworkComponent : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	//! Local player can request the placement of a buildable prefab
	void ACE_Trenches_RequestPlace(ResourceName prefabName, vector transform[4])
	{
		Rpc(RpcAsk_ACE_Trenches_RequestPlace, prefabName, transform);
	}

	//------------------------------------------------------------------------------------------------
	//! Spawn buildable prefab on the server
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_ACE_Trenches_RequestPlace(ResourceName prefabName, vector transform[4])
	{
		Resource res = Resource.Load(prefabName);
		if (!res.IsValid())
			return;
		
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform = transform;
		SCR_TerrainHelper.SnapAndOrientToTerrain(params.Transform);
		// Forces to spawn as buildable
		SCR_EditorLinkComponent.IgnoreSpawning(true);
		GetGame().SpawnEntityPrefab(res, null, params);
	}
}