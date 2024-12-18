//------------------------------------------------------------------------------------------------
modded class SCR_PlayerController : PlayerController
{
	//------------------------------------------------------------------------------------------------
	//! Request deletion of unreplicated entity from all machines
	//! Called from local player
	void ACE_RequestDeleteEntity(IEntity entity)
	{
		if (!entity)
			return;
		
		Rpc(RpcAsk_ACE_DeleteEntityByBits, ACE_EntityIdHelper.ToInt(entity.GetID()));
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_ACE_DeleteEntityByBits(array<int> bits)
	{
		ACE_LoadtimeEntityManager manager = ACE_LoadtimeEntityManager.GetInstance();
		if (!manager)
			return;
		
		manager.DeleteEntitiesByIdGlobal({EntityID.FromInt(bits[0], bits[1])});
	}
	
	//------------------------------------------------------------------------------------------------
	//! Request destruction of SCR_DestructibleEntity
	//! Called from local player
	void ACE_RequestDestroyEntity(SCR_DestructibleEntity entity, EDamageType damageType, vector hitPosDirNorm[4])
	{
		Rpc(RpcAsk_ACE_DestroyEntity, entity.GetID(), damageType, hitPosDirNorm);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_ACE_DestroyEntity(EntityID entityID, EDamageType damageType, vector hitPosDirNorm[4])
	{
		SCR_DestructibleEntity entity = SCR_DestructibleEntity.Cast(GetGame().GetWorld().FindEntityByID(entityID));
		if (!entity)
			return;
		
		entity.HandleDamage(damageType, entity.GetCurrentHealth(), hitPosDirNorm);
	}
}
