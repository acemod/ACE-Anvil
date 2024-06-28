//------------------------------------------------------------------------------------------------
//! Mine deletion user action
class ACE_Explosives_DeminingUserAction : ACE_ShovelDestroyUserAction
{	
	//------------------------------------------------------------------------------------------------
	//! Request deletion of the tree
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		SCR_EntityHelper.DeleteEntityAndChildren(pOwnerEntity);
	}
}