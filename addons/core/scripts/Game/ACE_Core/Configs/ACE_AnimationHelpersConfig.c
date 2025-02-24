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
[BaseContainerProps(), ACE_BaseContainerCustomTitlePrefabEnum("m_sPrefabName", ACE_EAnimationHelperID, "m_eID")]
class ACE_AnimationHelperConfigEntry
{
	[Attribute(desc: "Prefab name of the animation helper", params: "et")]
	protected ResourceName m_sPrefabName;
	
	protected ACE_EAnimationHelperID m_eID;
	
	//----------------------------------------------------------------------------------------
	ACE_EAnimationHelperID GetID()
	{
		if (m_eID == ACE_EAnimationHelperID.NONE)
		{
			Resource res = Resource.Load(m_sPrefabName);
			if (!res.IsValid())
				return m_eID;
			
			BaseContainer container = res.GetResource();
			container.Get("m_eID", m_eID);
		}
		
		return m_eID;
	}
	
	//----------------------------------------------------------------------------------------
	ResourceName GetPrefabName()
	{
		return m_sPrefabName;
	}
}
