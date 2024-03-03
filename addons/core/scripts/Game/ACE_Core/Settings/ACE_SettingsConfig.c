//------------------------------------------------------------------------------------------------
//! Base container for all settings
[BaseContainerProps(configRoot: true)]
class ACE_SettingsConfig
{
	[Attribute(desc: "Definitions of settings for all mods")]
	protected ref array<ref ACE_ModSettings> m_aModSettings;
	protected ref map<typename,ACE_ModSettings> m_mModSettingsMap;
	
	//------------------------------------------------------------------------------------------------
	//! Construct map for faster look-up of settings
	protected void InitMap()
	{
		m_mModSettingsMap = new map<typename,ACE_ModSettings>();
		
		foreach (ACE_ModSettings settings : m_aModSettings)
		{
			m_mModSettingsMap[settings.Type()] = settings;
		};
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return settings for all categories
	array<ref ACE_ModSettings> GetAllModSettings()
	{
		return m_aModSettings;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return settings for a mod
	ACE_ModSettings GetModSettings(typename modSettingsType)
	{
		if (!m_mModSettingsMap)
			InitMap();
		
		ACE_ModSettings settings;
		m_mModSettingsMap.Find(modSettingsType, settings);
		return settings;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Set settings for a mod
	void SetModSettings(ACE_ModSettings settings)
	{
		if (!m_mModSettingsMap)
			InitMap();
		
		m_mModSettingsMap[settings.Type()] = settings;
	}
}
