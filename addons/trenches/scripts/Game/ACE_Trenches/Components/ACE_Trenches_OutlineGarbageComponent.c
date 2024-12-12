//------------------------------------------------------------------------------------------------
class ACE_Trenches_OutlineGarbageComponentClass : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Unbuilt structures with this component get handled by the garbage system
//! It gets dropped from the queue once built
class ACE_Trenches_OutlineGarbageComponent : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	//! Add to garbage system queue on init
	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode() || !Replication.IsServer())
			return;
		
		SCR_GarbageSystem garbageSystem = SCR_GarbageSystem.GetByEntityWorld(owner);
		if (!garbageSystem)
			return;
		
		SCR_CampaignBuildingCompositionComponent compositionComponent = SCR_CampaignBuildingCompositionComponent.Cast(owner.FindComponent(SCR_CampaignBuildingCompositionComponent));
		if (!compositionComponent)
			return;
		
		garbageSystem.Insert(owner);
		compositionComponent.GetOnCompositionSpawned().Insert(OnCompositionSpawned);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Remove from garbage system queue when built
	protected void OnCompositionSpawned(bool compositionSpawned)
	{
		SCR_CampaignBuildingCompositionComponent compositionComponent = SCR_CampaignBuildingCompositionComponent.Cast(GetOwner().FindComponent(SCR_CampaignBuildingCompositionComponent));
		if (!compositionComponent)
			return;
		
		compositionComponent.GetOnCompositionSpawned().Remove(OnCompositionSpawned);
		
		SCR_GarbageSystem garbageSystem = SCR_GarbageSystem.GetByEntityWorld(GetOwner());
		if (!garbageSystem)
			return;
		
		garbageSystem.Withdraw(GetOwner());
	}
}
