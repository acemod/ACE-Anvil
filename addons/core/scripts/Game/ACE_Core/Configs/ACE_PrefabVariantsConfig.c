//------------------------------------------------------------------------------------------------
//! Container for storing a collection of prefab variants
[BaseContainerProps(configRoot: true)]
class ACE_PrefabVariantsConfig
{
	[Attribute(desc: "Collection of prefab variants.", params: "et", uiwidget: UIWidgets.ResourcePickerThumbnail)]
	protected ref array<ResourceName> m_aVariants;
	
	//------------------------------------------------------------------------------------------------
	array<ResourceName> GetVariants()
	{
		return m_aVariants;
	}
	
	//------------------------------------------------------------------------------------------------
	ResourceName GetRandomVariant()
	{
		if (m_aVariants.IsEmpty())
			return "";
		
		return m_aVariants.GetRandomElement();
	}
}
