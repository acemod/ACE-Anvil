//----------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true)]
class ACE_Trenches_Config
{
	[Attribute(defvalue: "{58F07022C12D0CF5}Assets/Editor/PlacingPreview/Preview.emat", desc: "Material for the outline if it can be placed", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "emat")]
	ResourceName m_sCanPlacePreviewMaterial;
	
	[Attribute(defvalue: "{14A9DCEA57D1C381}Assets/Conflict/CannotBuild.emat", desc: "Material for the outline if it cannot be placed", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "emat")]
	ResourceName m_sCannotPlacePreviewMaterial;
	
	[Attribute(desc: "Structures that can be placed with the E-tool")]
	ref array<ref ACE_Trenches_PlaceablePrefabConfig> m_aPlaceablePrefabConfigs;
	
	[Attribute(desc: "Surface game materials on which placement of terrain objects is not possible", uiwidget: UIWidgets.ResourceNamePicker, params: "gamemat")]
	ref array<ResourceName> m_aBlacklistedTerrainSurfaces;
}

//----------------------------------------------------------------------------------------
[BaseContainerProps(), BaseContainerCustomDoubleCheckIntResourceNameTitleField("m_bEnabled", "m_sPrefabName", 1, "%1", "DISABLED - %1")]
class ACE_Trenches_PlaceablePrefabConfig
{
	[Attribute(desc: "Prefab names of structures that can be placed with the E-tool", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "et")]
	ResourceName m_sPrefabName;
	
	[Attribute(defvalue: "true", desc: "Terrain objects can only be placed on terrain surfaces that are not blacklisted.")]
	bool m_bIsTerrainObject;
	
	[Attribute(defvalue: "true")]
	bool m_bEnabled;
}
