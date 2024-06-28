//------------------------------------------------------------------------------------------------
//! Explosive deletion user action
class ACE_Explosives_DisarmingUserAction : ACE_WrenchUserAction
{	
	
	//------------------------------------------------------------------------------------------------
	//! Request deletion of the explosive
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		SCR_EntityHelper.DeleteEntityAndChildren(pOwnerEntity);
	}
}