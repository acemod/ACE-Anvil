//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomEntityCatalogCatalog(EEntityCatalogType, "m_eEntityCatalogType", "m_aEntityEntryList", "m_aMultiLists")]
modded class SCR_EntityCatalogMultiList: SCR_EntityCatalog
{
	//------------------------------------------------------------------------------------------------
	protected override void InitCatalog()
	{
		m_aMultiLists.Sort();
		super.InitCatalog();
	}
}
