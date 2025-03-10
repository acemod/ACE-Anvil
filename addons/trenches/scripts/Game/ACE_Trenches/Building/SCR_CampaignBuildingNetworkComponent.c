//------------------------------------------------------------------------------------------------
//! Add request for spawning a buildable prefab
//! Attached to the player controller
modded class SCR_CampaignBuildingNetworkComponent : ScriptComponent
{
	protected ACE_Trenches_Config m_ACE_Trenches_Config;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		if (!GetGame().InPlayMode())
			return;
		
		SCR_CampaignBuildingManagerComponent manager = SCR_CampaignBuildingManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_CampaignBuildingManagerComponent));
		if (!manager)
			return;
		
		m_ACE_Trenches_Config = manager.ACE_Trenches_GetConfig();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Local player can request the placement of a buildable prefab
	void ACE_Trenches_RequestPlace(int prefabID, vector transform[4], IEntity provider)
	{
		RplId providerID = RplId.Invalid();
		
		RplComponent providerRpl = RplComponent.Cast(provider.FindComponent(RplComponent));
		if (providerRpl)
			providerID = providerRpl.Id();
		
		Rpc(RpcAsk_ACE_Trenches_RequestPlace, prefabID, transform, providerID);
	}

	//------------------------------------------------------------------------------------------------
	//! Spawn buildable prefab on the server
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_ACE_Trenches_RequestPlace(int prefabID, vector transform[4], RplId providerID)
	{
		ACE_Trenches_PlaceablePrefabConfig prefabConfig = m_ACE_Trenches_Config.GetPlaceablePrefabConfig(prefabID);
		if (!prefabConfig)
			return;
		
		SCR_ResourceConsumer consumer = ACE_Trenches_GetSupplyConsumerFromProviderID(providerID);
		if (consumer && consumer.RequestConsumtion(prefabConfig.GetSupplyCost()).GetReason() != EResourceReason.SUFFICIENT)
			return;
		
		Resource res = Resource.Load(prefabConfig.GetPrefabName());
		if (!res.IsValid())
			return;
		
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform = transform;
		// Forces to spawn as buildable
		SCR_EditorLinkComponent.IgnoreSpawning(true);
		
		IEntity asset = GetGame().SpawnEntityPrefab(res, null, params);
		if (!asset)
			return;
		
		SCR_CampaignBuildingCompositionComponent compositionComponent = SCR_CampaignBuildingCompositionComponent.Cast(asset.FindComponent(SCR_CampaignBuildingCompositionComponent));
		if (!compositionComponent)
			return;
		
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetOwner());
		if (!playerController)
			return;
		
		compositionComponent.SetBuilderId(playerController.GetPlayerId());
		
		SCR_CampaignBuildingLayoutComponent layoutComponent = SCR_CampaignBuildingLayoutComponent.Cast(asset.GetChildren().FindComponent(SCR_CampaignBuildingLayoutComponent));
		if (!layoutComponent)
			return;
		
		ACE_Treches_Settings settings = ACE_SettingsHelperT<ACE_Treches_Settings>.GetModSettings();
		if (!settings)
			return;
		
		layoutComponent.ACE_Trenches_SetToBuildValue(settings.m_fBuildTimeScale * layoutComponent.GetToBuildValue());
	}
	
	//------------------------------------------------------------------------------------------------
	protected SCR_ResourceConsumer ACE_Trenches_GetSupplyConsumerFromProviderID(RplId providerID)
	{
		RplComponent providerRpl = RplComponent.Cast(Replication.FindItem(providerID));
		if (!providerRpl)
			return null;
		
		IEntity provider = providerRpl.GetEntity();
		if (!provider)
			return null;
		
		SCR_ResourceComponent resourceComponent = SCR_ResourceComponent.Cast(provider.FindComponent(SCR_ResourceComponent));
		if (!resourceComponent || !resourceComponent.IsResourceTypeEnabled(EResourceType.SUPPLIES))
			return null;
		
		return resourceComponent.GetConsumer(EResourceGeneratorID.DEFAULT, EResourceType.SUPPLIES);
	}
}
