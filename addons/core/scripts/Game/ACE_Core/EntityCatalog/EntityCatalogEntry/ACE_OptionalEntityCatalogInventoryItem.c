//------------------------------------------------------------------------------------------------
//! Same as vanilla, but additional addon GUIDs can be provided for checking
[BaseContainerProps(), SCR_BaseContainerCustomInventoryCatalogEntry("m_sEntityPrefab", "m_aEntityDataList", "m_bEnabled")]
class ACE_OptionalEntityCatalogInventoryItem : SCR_EntityCatalogInventoryItem
{
	[Attribute(desc: "Addon GUID required for this entry. Used for checking optional dependencies.")]
	protected string m_sRequiredAddonGUID;
	
	//------------------------------------------------------------------------------------------------
	override bool IsEnabled()
	{
		if (!super.IsEnabled())
			return false;
		
		if (m_sRequiredAddonGUID.IsEmpty())
			return true;
		
		array<string> loadedAddonGUIDs = {};
		GameProject.GetLoadedAddons(loadedAddonGUIDs);
		return loadedAddonGUIDs.Contains(m_sRequiredAddonGUID);
	}
}
