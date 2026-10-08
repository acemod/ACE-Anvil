//------------------------------------------------------------------------------------------------
modded class SCR_PlayerController : PlayerController
{
	protected static const float ACE_MAX_DELETED_DISTANCE_M = 5; // Maximum distance for deletion requests to be granted
	
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
		IEntity entity = GetGame().GetWorld().FindEntityByID(entityID);
		if (!entity)
			return;
		
		if (!ACE_IsDeletionGrantedByServer(entity))
			return;
		
		ACE_LoadtimeEntityManager manager = ACE_LoadtimeEntityManager.GetInstance();
		if (!manager)
			return;
		
		manager.DeleteEntitiesByIdGlobal({entityID});
	}
	
	//------------------------------------------------------------------------------------------------
	//! Request destruction of entity
	//! Called from local player
	void ACE_RequestDestroyEntity(IEntity entity, vector hitPosDirNorm[3], int deletionDelayMS = -1)
	{
		RplId rplId = SCR_EntityHelper.EntityToRplId(entity);
		if (rplId.IsValid())
			Rpc(RpcAsk_ACE_DestroyRplEntity, rplId, hitPosDirNorm, deletionDelayMS);
		else
			Rpc(RpcAsk_ACE_DestroyDestructibleEntity, entity.GetID(), hitPosDirNorm, deletionDelayMS);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_ACE_DestroyDestructibleEntity(EntityID entityID, vector hitPosDirNorm[3], int deletionDelayMS)
	{
		SCR_DestructibleEntity entity = SCR_DestructibleEntity.Cast(GetGame().GetWorld().FindEntityByID(entityID));
		if (!entity)
			return;
		
		if (!ACE_IsDeletionGrantedByServer(entity))
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
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_ACE_DestroyRplEntity(RplId entityID, vector hitPosDirNorm[3], int deletionDelayMS)
	{
		IEntity entity = SCR_EntityHelper.RplIdToEntity(entityID);
		if (!entity)
			return;
		
		if (!ACE_IsDeletionGrantedByServer(entity))
			return;
		
		SCR_DamageManagerComponent damageManager = SCR_DamageManagerComponent.Cast(entity.FindComponent(DamageManagerComponent));
		if (!damageManager)
			return;
		
		float health = damageManager.GetHealth();
		if (health > 0)
			damageManager.Kill(damageManager.GetInstigator());
		
		if (deletionDelayMS <= 0)
			return;
		
		ACE_LoadtimeEntityManager manager = ACE_LoadtimeEntityManager.GetInstance();
		if (!manager)
			return;
		
		// Delete immediately if it was already destroyed
		if (health <= 0)
			SCR_EntityHelper.DeleteEntityAndChildren(entity);
		else
			GetGame().GetCallqueue().CallLater(SCR_EntityHelper.DeleteEntityAndChildren, deletionDelayMS, false, entity);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check whether player is allowed deleting the entity
	protected bool ACE_IsDeletionGrantedByServer(IEntity entityToDelete)
	{
		// Only physical loadtime entities can be deleted
		if (!entityToDelete || !entityToDelete.IsLoaded() || !entityToDelete.GetVObject())
			return false;
		
		SCR_ChimeraCharacter playerChar = SCR_ChimeraCharacter.Cast(GetControlledEntity());
		if (!playerChar)
			return false;
		
		// Can only delete nearby entities
		return (vector.Distance(playerChar.GetOrigin(), entityToDelete.GetOrigin()) <= ACE_MAX_DELETED_DISTANCE_M);
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
