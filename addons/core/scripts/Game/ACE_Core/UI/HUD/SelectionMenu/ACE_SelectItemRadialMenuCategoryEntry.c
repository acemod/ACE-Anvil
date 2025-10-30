//------------------------------------------------------------------------------------------------
//! Creates radial submenu based on items of the local player matching m_pItemSearchPredicate
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class ACE_SelectItemRadialMenuCategoryEntry : ACE_SelectionMenuCategoryEntry
{
	[Attribute(desc: "Predicate that items should match")]
	protected ref ACE_InventorySearchPredicate m_pItemSearchPredicate;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPerform()
	{
		m_aEntries.Clear();
		
		IEntity char = SCR_PlayerController.GetLocalControlledEntity();
		if (!char)
			return;
		
		SCR_InventoryStorageManagerComponent storageManager = SCR_InventoryStorageManagerComponent.Cast(char.FindComponent(SCR_InventoryStorageManagerComponent));		
		if (!storageManager)
			return;
		
		array<IEntity> items = {};
		storageManager.FindItems(items, m_pItemSearchPredicate);
		SCR_Sorting<IEntity, ACE_CompareItemName>.HeapSort(items, false);
		
		// Make sure we only add each
		set<EntityPrefabData> uniquePrefabDataSet = new set<EntityPrefabData>();
		
		foreach (IEntity item : items)
		{
			EntityPrefabData prefabData = item.GetPrefabData();
			if (!prefabData)
				continue;
			
			if (uniquePrefabDataSet.Find(prefabData) >= 0)
				continue;
			
			m_aEntries.Insert(new ACE_SelectItemRadialMenuEntry(item));
			uniquePrefabDataSet.Insert(prefabData);
		}
	}
}
