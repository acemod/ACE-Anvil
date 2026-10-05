//------------------------------------------------------------------------------------------------
//! User action for destrying entities like a tree
class ACE_DestroyEntityUserAction : ACE_ContinousGadgetUserAction
{
	[Attribute(defvalue: "3000", desc: "How many miliseconds after destruction should the entity get deleted.")]
	protected int m_iDeletionDelayMS;
	
	//------------------------------------------------------------------------------------------------
	//! Request deletion of the tree
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_PlayerController userCtrl = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!userCtrl)
			return;
		
		ACE_ActionsHelperEntity helper = ACE_ActionsHelperEntity.Cast(GetOwner());
		if (!helper)
			return;
		
		IEntity entity = helper.GetAssociatedEntity();
		if (!entity)
			return;
		
		if (IsEntityDestructible(entity))
		{
			vector hitPosDirNorm[3];
			hitPosDirNorm[0] = pOwnerEntity.GetOrigin();
			userCtrl.ACE_RequestDestroyEntity(entity, hitPosDirNorm, m_iDeletionDelayMS);
		}
		else
		{
			userCtrl.ACE_RequestDeleteEntity(entity);
		}
		
		delete helper;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool IsEntityDestructible(IEntity entity)
	{
		DestructibleEntity destructible = DestructibleEntity.Cast(entity);
		if (destructible)
		{
			bool enabled;
			BaseContainer container = entity.GetPrefabData().GetPrefab();
			return (container && container.Get("Enabled", enabled) && enabled);
		}
		
		DamageManagerComponent damageManager = DamageManagerComponent.Cast(entity.FindComponent(DamageManagerComponent));
		if (damageManager)
			return true;
		
		return false;
	}

	//------------------------------------------------------------------------------------------------
	//! For entities that have no RplComponent, only local scripts will work
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
}
