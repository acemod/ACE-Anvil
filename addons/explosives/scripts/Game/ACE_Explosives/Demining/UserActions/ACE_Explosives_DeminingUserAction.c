//------------------------------------------------------------------------------------------------
//! Explosive deletion user action
class ACE_Explosives_DeminingUserAction : ACE_WrenchDestroyUserAction
{
	
	//------------------------------------------------------------------------------------------------
	//! Request deletion of the explosive
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		SCR_EntityHelper.DeleteEntityAndChildren(pOwnerEntity);
	}
}