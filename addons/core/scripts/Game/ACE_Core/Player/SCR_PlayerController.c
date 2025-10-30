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
		
		Rpc(RpcAsk_ACE_DeleteEntityByID, entity.GetID());
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_ACE_DeleteEntityByID(EntityID entityID)
	{
		ACE_LoadtimeEntityManager manager = ACE_LoadtimeEntityManager.GetInstance();
		if (!manager)
			return;
		
		manager.DeleteEntitiesByIdGlobal({entityID});
	}
	
	//------------------------------------------------------------------------------------------------
	//! Request destruction of SCR_DestructibleEntity
	//! Called from local player
	void ACE_RequestDestroyEntity(SCR_DestructibleEntity entity, vector hitPosDirNorm[3], int deletionDelayMS = -1)
	{
		Rpc(RpcAsk_ACE_DestroyEntity, entity.GetID(), hitPosDirNorm, deletionDelayMS);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_ACE_DestroyEntity(EntityID entityID, vector hitPosDirNorm[3], int deletionDelayMS)
	{
		SCR_DestructibleEntity entity = SCR_DestructibleEntity.Cast(GetGame().GetWorld().FindEntityByID(entityID));
		if (!entity)
			return;
		
		float health = entity.GetCurrentHealth();
		if (health > 0)
			entity.HandleDamage(EDamageType.TRUE, health, hitPosDirNorm);
		
		if (deletionDelayMS <= 0)
			return;
		
		ACE_LoadtimeEntityManager manager = ACE_LoadtimeEntityManager.GetInstance();
		if (!manager)
			return;
		
		// Delete immediately if it was already destroyed
		if (health <= 0)
			manager.DeleteEntitiesByIdGlobal({entity.GetID()});
		else
			GetGame().GetCallqueue().CallLater(manager.DeleteEntitiesByIdGlobal, deletionDelayMS, false, {entity.GetID()});
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_RequestAnimateWithHelperCompartment(ACE_EAnimationHelperID helperID)
	{
		Rpc(RpcAsk_ACE_AnimateWithHelperCompartment, helperID);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_ACE_AnimateWithHelperCompartment(ACE_EAnimationHelperID helperID)
	{
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(GetControlledEntity());
		// Skip if no character or character inside something else
		if (!char || char.GetParent())
			return;
		
		ACE_AnimationTools.AnimateWithHelperCompartment(helperID, char);
	}
}
