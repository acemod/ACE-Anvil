//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomEntityCatalogMultiListEntry("m_sIdentifier", "m_aEntities")]
modded class SCR_EntityCatalogMultiListEntry
{
	[SortAttribute(), Attribute(desc: "Sort priority of the entry. Lower value means higher priority.")]
	protected int m_iACE_Priority;
}
