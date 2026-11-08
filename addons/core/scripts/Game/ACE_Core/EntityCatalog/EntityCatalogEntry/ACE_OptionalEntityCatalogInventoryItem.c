//------------------------------------------------------------------------------------------------
//! Same as vanilla, but additional addon GUIDs can be provided for checks
[BaseContainerProps(), SCR_BaseContainerCustomInventoryCatalogEntry("m_sEntityPrefab", "m_aEntityDataList", "m_bEnabled")]
class ACE_OptionalEntityCatalogInventoryItem : SCR_EntityCatalogInventoryItem
{
	[Attribute(desc: "Addon GUID required for this entry to be enabled. Used for checking optional dependencies.")]
	protected string m_sRequiredAddonGUID;
	
	[Attribute(desc: "Addon GUID which disables this entry. Used for checking optional dependencies.")]
	protected string m_sDisallowedAddonGUID;
	
	//------------------------------------------------------------------------------------------------
	override bool IsEnabled()
	{
		if (!super.IsEnabled())
			return false;
		
		array<string> loadedAddonGUIDs = {};
		GameProject.GetLoadedAddons(loadedAddonGUIDs);
		
		if (!m_sDisallowedAddonGUID.IsEmpty() && loadedAddonGUIDs.Contains(m_sDisallowedAddonGUID))
			return false;
		
		return (m_sRequiredAddonGUID.IsEmpty() || loadedAddonGUIDs.Contains(m_sRequiredAddonGUID));
	}
}
