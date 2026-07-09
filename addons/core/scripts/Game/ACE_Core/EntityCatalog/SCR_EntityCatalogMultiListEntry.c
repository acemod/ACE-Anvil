//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomEntityCatalogMultiListEntry("m_sIdentifier", "m_aEntities")]
modded class SCR_EntityCatalogMultiListEntry
{
	[SortAttribute(), Attribute(desc: "Sort priority of the entry. Lower value means higher priority.")]
	int m_iACE_Priority;
}

//------------------------------------------------------------------------------------------------
class ACE_CompateEntityCatalogMultiListEntryPriority : SCR_SortCompare<SCR_EntityCatalogMultiListEntry>
{
	//------------------------------------------------------------------------------------------------
	override static int Compare(SCR_EntityCatalogMultiListEntry left, SCR_EntityCatalogMultiListEntry right)
	{
		return left.m_iACE_Priority < right.m_iACE_Priority;
	}
}
