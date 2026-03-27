//------------------------------------------------------------------------------------------------
class ACE_Trenches_BuildingCompositionComponentClass : SCR_CampaignBuildingCompositionComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_Trenches_BuildingCompositionComponent : SCR_CampaignBuildingCompositionComponent
{
	//------------------------------------------------------------------------------------------------
	override protected void SetIsCompositionSpawned()
	{
		super.SetIsCompositionSpawned();
		
		if (!Replication.IsServer())
			return;
		
		SnapItemsInOutline();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Move all items inside trench on top of it
	protected void SnapItemsInOutline()
	{
		IEntity outlineEntity = GetOwner().GetChildren();
		
		TraceParam params = new TraceParam();
		params.Flags = TraceFlags.ENTS;
		params.Include = outlineEntity;
		
		vector mins, maxs;
		outlineEntity.GetBounds(mins, maxs);
		vector ownerTransform[4];
		outlineEntity.GetWorldTransform(ownerTransform);
		vector up = 1.1 * (maxs[1] - mins[0]) * ownerTransform[1];
		
		foreach (IEntity item : ACE_QueryEntitiesTools.GetAllEntitiesInOBB(mins, maxs, ownerTransform))
		{
			if (item != item.GetRootParent() || !item.FindComponent(InventoryItemComponent))
				continue;
			
			vector transform[4];
			item.GetWorldTransform(transform);
			params.Start = transform[3];
			params.End = params.Start + up;
			
			float progress = outlineEntity.GetWorld().TraceMove(params, null);
			if (progress < 1)
			{
				transform[3] = transform[3] + progress * up;
				
				SCR_EditableEntityComponent editable = SCR_EditableEntityComponent.GetEditableEntity(item);
				if (editable)
				{
					editable.SetTransform(transform);
				}
				else
				{
					RplComponent itemRpl = RplComponent.Cast(item.FindComponent(RplComponent));
					if (!itemRpl)
						continue;
					
					RplId itemID = itemRpl.Id();
					RpcDo_SetItemTransformBroadcast(itemID, transform);
					Rpc(RpcDo_SetItemTransformBroadcast, itemID, transform);
				}
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_SetItemTransformBroadcast(RplId itemID, vector transform[4])
	{
		RplComponent itemRpl = RplComponent.Cast(Replication.FindItem(itemID));
		if (!itemRpl)
			return;
		
		IEntity item = itemRpl.GetEntity();
		if (!item)
			return;
		
		item.SetWorldTransform(transform);
		item.Update();
	}
}
