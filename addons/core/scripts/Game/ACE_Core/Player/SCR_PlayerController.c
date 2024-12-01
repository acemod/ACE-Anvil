//------------------------------------------------------------------------------------------------
modded class SCR_PlayerController : PlayerController
{
	//------------------------------------------------------------------------------------------------
	//! Request deletion of unreplicated entity from all machines
	//! Called from local player
	void ACE_RequestDeleteEntity(IEntity entity)
	{
		Rpc(RpcAsk_ACE_DeleteEntityByID, entity.GetID());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Request deletion of unreplicated entity from all machines
	//! Called from server
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_ACE_DeleteEntityByID(EntityID entityID)
	{
		ACE_LoadtimeEntityManager manager = ACE_LoadtimeEntityManager.GetInstance();
		if (!manager)
			return;
		
		manager.DeleteEntityByIdGlobal(entityID);
	}
}
