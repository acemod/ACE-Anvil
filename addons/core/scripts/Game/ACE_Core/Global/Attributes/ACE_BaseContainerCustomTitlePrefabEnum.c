//----------------------------------------------------------------------------------------
//! Attribute for setting enum property from the prefab of a resource path property as custom title
class ACE_BaseContainerCustomTitlePrefabEnum : BaseContainerCustomTitle
{
	protected string m_sPrefabPropertyName;
	protected typename m_tEnumType;
	protected string m_sEnumPropertyName;
	protected string m_sFormat;

	//------------------------------------------------------------------------------------------------
	void ACE_BaseContainerCustomTitlePrefabEnum(string prefabPropertyName, typename enumType, string enumPropertyName, string format = "%1")
	{
		m_sPrefabPropertyName = prefabPropertyName;
		m_tEnumType = enumType;
		m_sEnumPropertyName = enumPropertyName;
		m_sFormat = format;
	}

	//------------------------------------------------------------------------------------------------
	override bool _WB_GetCustomTitle(BaseContainer source, out string title)
	{
		ResourceName resName;
		if (!source.Get(m_sPrefabPropertyName, resName))
			return false;
		
		Resource res = Resource.Load(resName);
		if (!res.IsValid())
			return false;
		
		BaseContainer prefabSource = res.GetResource();
		
		int enumValue;
		if (!prefabSource.Get(m_sEnumPropertyName, enumValue))
			return false;

		title = string.Format(m_sFormat, typename.EnumToString(m_tEnumType, enumValue));
		return true;
	}
}
