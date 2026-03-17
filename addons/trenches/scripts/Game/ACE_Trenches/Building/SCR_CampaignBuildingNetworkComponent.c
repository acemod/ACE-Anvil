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

	IEntity asset = GetGame().SpawnEntityPrefab(res, null, params);
	if (!asset)
	    return;

	SCR_CampaignBuildingCompositionComponent compositionComponent =
	    SCR_CampaignBuildingCompositionComponent.Cast(asset.FindComponent(SCR_CampaignBuildingCompositionComponent));
	if (!compositionComponent)
	    return;

	SCR_PlayerController playerController = SCR_PlayerController.Cast(GetOwner());
	if (!playerController)
	    return;

	compositionComponent.SetBuilderId(playerController.GetPlayerId());
    }
}