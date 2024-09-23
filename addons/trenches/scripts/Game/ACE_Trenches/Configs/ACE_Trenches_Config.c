//----------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true)]
class ACE_Trenches_Config
{
	[Attribute(defvalue: "{58F07022C12D0CF5}Assets/Editor/PlacingPreview/Preview.emat", desc: "Material for the outline if it can be placed", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "emat")]
	ResourceName m_sCanPlacePreviewMaterial;
	
	[Attribute(defvalue: "{14A9DCEA57D1C381}Assets/Conflict/CannotBuild.emat", desc: "Material for the outline if it cannot be placed", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "emat")]
	ResourceName m_sCannotPlacePreviewMaterial;
	
	[Attribute(desc: "Prefab names of structures that can be placed with the E-tool", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "et")]
	ref array<ResourceName> m_aPlaceablePrefabNames;
	
	[Attribute(desc: "Surface game materials on which placement is not possible", uiwidget: UIWidgets.ResourceNamePicker, params: "gamemat")]
	ref array<ResourceName> m_aBlacklistedSurfaces;
}
