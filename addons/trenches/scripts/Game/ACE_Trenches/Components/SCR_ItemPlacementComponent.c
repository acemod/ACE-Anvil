//------------------------------------------------------------------------------------------------
modded class SCR_ItemPlacementComponent : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	//! If the item to be placed is the E-tool, we don't place it, but rather spawn the selected trench variant
	override protected void StartPlaceItem()
	{
		SCR_CampaignBuildingGadgetToolComponent gadgetComponent = SCR_CampaignBuildingGadgetToolComponent.Cast(m_PlacedItem.FindComponent(SCR_CampaignBuildingGadgetToolComponent));
		if (!gadgetComponent)
			return super.StartPlaceItem();
		
		if (m_eCantPlaceReason > 0)
		{
			SCR_NotificationsComponent.SendLocal(m_eCantPlaceReason);
			return;
		}
		
		RplComponent gadgetRpl = RplComponent.Cast(m_PlacedItem.FindComponent(RplComponent));
		if (!gadgetRpl)
			return;
		
		Rpc(RpcDo_ACE_Trenches_RequestSpawnTrench, gadgetRpl.Id(), gadgetComponent.ACE_Trenches_GetCurrentVariantID(), m_vCurrentMat);
		SCR_UISoundEntity.SoundEvent("SOUND_E_ACE_TRENCH_BUILD");
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcDo_ACE_Trenches_RequestSpawnTrench(int gadgetID, int variantID, vector transform[4])
	{
		RplComponent gadgetRpl = RplComponent.Cast(Replication.FindItem(gadgetID));
		if (!gadgetRpl)
			return;
		
		IEntity gadget = gadgetRpl.GetEntity();
		if (!gadget)
			return;
		
		SCR_MultiPartDeployableItemComponent dataComponent = SCR_MultiPartDeployableItemComponent.Cast(gadget.FindComponent(SCR_MultiPartDeployableItemComponent));
		if (!dataComponent)
			return;
		
		SCR_MultiPartDeployableItemComponentClass data = SCR_MultiPartDeployableItemComponentClass.Cast(dataComponent.GetComponentData(gadget));
		if (!data)
			return;
		
		SCR_DeployableVariantContainer container = data.GetVariantContainer(variantID);
		if (!container)
			return;
		
		Resource res = Resource.Load(container.GetReplacementPrefab());
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
	}
}
