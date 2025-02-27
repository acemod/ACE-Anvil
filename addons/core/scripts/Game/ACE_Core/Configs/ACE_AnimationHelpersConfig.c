//----------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true)]
class ACE_AnimationHelpersConfig
{
	[Attribute(desc: "Registered animation helpers")]
	protected ref array<ref ACE_AnimationHelperConfigEntry> m_aEntries;
	
	//----------------------------------------------------------------------------------------
	ResourceName GetPrefabName(ACE_EAnimationHelperID id)
	{
		foreach (ACE_AnimationHelperConfigEntry entry : m_aEntries)
		{
			if (entry.GetID() == id)
				return entry.GetPrefabName();
		}
		
		return "";
	}
}

//----------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_EAnimationHelperID, "m_eID")]
class ACE_AnimationHelperConfigEntry
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, desc: "ID of the animation helper",  enums: ParamEnumArray.FromEnum(ACE_EAnimationHelperID))]
	protected ACE_EAnimationHelperID m_eID;
		
	[Attribute(desc: "Prefab name of the animation helper", params: "et")]
	protected ResourceName m_sPrefabName;
		
	//----------------------------------------------------------------------------------------
	ACE_EAnimationHelperID GetID()
	{
		return m_eID;
	}
	
	//----------------------------------------------------------------------------------------
	ResourceName GetPrefabName()
	{
		return m_sPrefabName;
	}
}
