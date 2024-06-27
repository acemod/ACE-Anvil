//------------------------------------------------------------------------------------------------
//! Tree deletion user action
class ACE_Chopping_UserAction : ACE_ShovelDestroyUserAction
{
	
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
		
		IEntity plant = helper.GetAssociatedPlant();
		if (!plant)
			return;
		
		userCtrl.ACE_DeleteEntityAtPosition(plant.GetOrigin());
		SCR_EntityHelper.DeleteEntityAndChildren(helper);
	}
}