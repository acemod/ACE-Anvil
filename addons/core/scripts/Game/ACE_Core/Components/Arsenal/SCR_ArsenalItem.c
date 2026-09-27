//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), BaseContainerCustomCheckIntTitleField("m_bEnabled", "Arsenal Data", "DISABLED - Arsenal Data", 1)]
modded class SCR_ArsenalItem : SCR_BaseEntityCatalogData
{
	//------------------------------------------------------------------------------------------------
	//! Get the calculated cost of filled containers
	override void PostInitData(notnull SCR_EntityCatalogEntry entry)
	{
		super.PostInitData(entry);
		
		if (!m_ItemResource || !m_ItemResource.IsValid())
			return;
		
		if (m_eItemMode != SCR_EArsenalItemMode.DEFAULT && m_eItemMode != SCR_EArsenalItemMode.SUPPORT_STATION)
			return;
		
		ACE_AddAdditionalCosts(entry, m_ItemResource, m_aAdditionalCosts, m_aNonArsenalAdditionalCosts);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Add calculated cost of stored items in container
	protected void ACE_AddAdditionalCosts(notnull SCR_EntityCatalogEntry entry, Resource resource, inout array<SCR_ArsenalItem> arsenalCosts, inout array<SCR_NonArsenalItemCostCatalogData> nonArsenalCosts)
	{
		IEntitySource entitySource = SCR_BaseContainerTools.FindEntitySource(resource);
		if (!entitySource)
			return;
		
		IEntityComponentSource componentSource = SCR_BaseContainerTools.FindComponentSource(entitySource, BaseUniversalInventoryStorageComponent);
		if (!componentSource)
			return;
		
		BaseContainerList slots = componentSource.GetObjectArray("InitialStorageSlots");
		for (int i, count = slots.Count(); i < count; i++)
		{
			ACE_AddAdditionalSlotCost(entry, slots.Get(i), 1, arsenalCosts, nonArsenalCosts);
		}
		
		BaseContainerList multiSlots = componentSource.GetObjectArray("MultiSlots");
		for (int i, count = multiSlots.Count(); i < count; i++)
		{
			BaseContainer multiSlot = multiSlots.Get(i);
			
			int slotCount;
			multiSlot.Get("NumSlots", slotCount);
			if (slotCount <= 0)
				continue;
			
			BaseContainer slotTemplate = multiSlot.GetObject("SlotTemplate");
			if (!slotTemplate)
				continue;
			
			ACE_AddAdditionalSlotCost(entry, slotTemplate, slotCount, arsenalCosts, nonArsenalCosts);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Add cost of item slot
	protected void ACE_AddAdditionalSlotCost(notnull SCR_EntityCatalogEntry entry, BaseContainer slotTemplate, int slotCount, inout array<SCR_ArsenalItem> arsenalCosts, inout array<SCR_NonArsenalItemCostCatalogData> nonArsenalCosts)
	{
		bool isEnabled;
		slotTemplate.Get("Enabled", isEnabled);
		if (!isEnabled)
			return;
		
		ResourceName storedItemName;
		slotTemplate.Get("Prefab", storedItemName);
		if (storedItemName.IsEmpty())
			return;
		
		SCR_EntityCatalogEntry storedItemEntry = entry.GetCatalogParent().GetEntryWithPrefab(storedItemName);
		if (!storedItemEntry)
			return;
		
		array<SCR_BaseEntityCatalogData> entityDataList = {};
		storedItemEntry.GetEntityDataList(entityDataList);
		foreach (SCR_BaseEntityCatalogData data : entityDataList)
		{
			SCR_ArsenalItem arsenalData = SCR_ArsenalItem.Cast(data);
			if (arsenalData)
			{
				if (!arsenalCosts)
					arsenalCosts = {};
				
				for (int i; i < slotCount; i++)
				{
					arsenalCosts.Insert(arsenalData);
				}
				
				break;
			}
			
			SCR_NonArsenalItemCostCatalogData nonArsenalData = SCR_NonArsenalItemCostCatalogData.Cast(data);
			if (nonArsenalData)
			{
				if (!nonArsenalCosts)
					nonArsenalCosts = {};
				
				for (int i; i < slotCount; i++)
				{
					nonArsenalCosts.Insert(nonArsenalData);
				}
				
				break;
			}
		}
	}
}
