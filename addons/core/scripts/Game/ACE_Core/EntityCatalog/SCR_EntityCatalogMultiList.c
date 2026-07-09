//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomEntityCatalogCatalog(EEntityCatalogType, "m_eEntityCatalogType", "m_aEntityEntryList", "m_aMultiLists")]
modded class SCR_EntityCatalogMultiList: SCR_EntityCatalog
{
	//------------------------------------------------------------------------------------------------
	protected override void InitCatalog()
	{
		ACE_Sorting<SCR_EntityCatalogMultiListEntry, array<ref SCR_EntityCatalogMultiListEntry>, ACE_CompateEntityCatalogMultiListEntryPriority>.InsertSort(m_aMultiLists);		
		super.InitCatalog();
	}
}
