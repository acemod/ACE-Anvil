//------------------------------------------------------------------------------------------------
modded class SCR_InventoryStorageManagerComponent : ScriptedInventoryStorageManagerComponent
{
	protected ref map<SCR_EConsumableType, int> m_mACE_Medical_ConsumableCounts = new map<SCR_EConsumableType, int>();

	//------------------------------------------------------------------------------------------------
	//! Update consumable item count
	override protected void OnItemAdded(BaseInventoryStorageComponent storageOwner, IEntity item)
	{
		super.OnItemAdded(storageOwner, item);

		SCR_ConsumableItemComponent consumable = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
		if (!consumable)
			return;
		
		SCR_EConsumableType type = consumable.GetConsumableType();
		m_mACE_Medical_ConsumableCounts[type] = ACE_Medical_GetConsumableCount(type) + 1;
	}

	//------------------------------------------------------------------------------------------------
	//! Update consumable item count
	override protected void OnItemRemoved(BaseInventoryStorageComponent storageOwner, IEntity item)
	{
		super.OnItemRemoved(storageOwner, item);

		SCR_ConsumableItemComponent consumable = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
		if (!consumable)
			return;
		
		SCR_EConsumableType type = consumable.GetConsumableType();
		m_mACE_Medical_ConsumableCounts[type] = ACE_Medical_GetConsumableCount(type) - 1;
	}

	//------------------------------------------------------------------------------------------------
	//! Return consumable item count
	int ACE_Medical_GetConsumableCount(SCR_EConsumableType type)
	{
		int count;
		m_mACE_Medical_ConsumableCounts.Find(type, count);
		return count;
	}
}
