//------------------------------------------------------------------------------------------------
//! Tree deletion user action
class ACE_Chopping_UserAction : ACE_ShovelUserAction
{
	protected static const int DELETE_FALLING_TREE_DELAY_MS = 10000;
	
	//------------------------------------------------------------------------------------------------
	//! Request deletion of the tree
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_PlayerController userCtrl = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!userCtrl)
			return;
		
		ACE_Chopping_HelperEntity helper = ACE_Chopping_HelperEntity.Cast(GetOwner());
		if (!helper)
			return;
		
		Tree plant = Tree.Cast(helper.GetAssociatedPlant());
		if (!plant)
			return;
		
		bool enabled;
		BaseContainer container = plant.GetPrefabData().GetPrefab();
		
		if (container && container.Get("Enabled", enabled) && enabled)
		{
			vector transform[4];
			pUserEntity.GetWorldTransform(transform);
			userCtrl.ACE_RequestDestroyEntity(plant, EDamageType.MELEE, transform);
			GetGame().GetCallqueue().CallLater(userCtrl.ACE_RequestDeleteEntity, DELETE_FALLING_TREE_DELAY_MS, false, plant);
		}
		else
		{
			userCtrl.ACE_RequestDeleteEntity(plant);
		}
		
		delete helper;
	}

	//------------------------------------------------------------------------------------------------
	//! For entities that have no RplComponent, only local scripts will work
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
}
