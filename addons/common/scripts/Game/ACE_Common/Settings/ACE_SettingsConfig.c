//------------------------------------------------------------------------------------------------
//! Base container for all settings
[BaseContainerProps(configRoot: true)]
class ACE_SettingsConfig
{
	[Attribute(desc: "Definitions of settings for all mods")]
	protected ref array<ref ACE_SettingsCategoryConfig> m_aSettingsCategories;
	protected ref map<typename,ACE_SettingsCategoryConfig> m_mSettingsCategories;
	
	//------------------------------------------------------------------------------------------------
	//! Initializes config:
	//! -  Construct map for faster look-up of configs
	protected void Init()
	{
		m_mSettingsCategories = new map<typename,ACE_SettingsCategoryConfig>();
		
		foreach (ACE_SettingsCategoryConfig config : m_aSettingsCategories)
		{
			m_mSettingsCategories[config.Type()] = config;
		};
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return settings for all categories
	array<ref ACE_SettingsCategoryConfig> GetSettingsCategoriesConfigs()
	{
		return m_aSettingsCategories;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return settings for a category
	ACE_SettingsCategoryConfig GetSettingsCategoryConfig(typename categoryType)
	{
		if (!m_mSettingsCategories)
			Init();
		
		ACE_SettingsCategoryConfig config;
		m_mSettingsCategories.Find(categoryType, config);
		return config;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Set settings for a category
	void SetSettingsCategoryConfig(ACE_SettingsCategoryConfig config)
	{
		if (!m_mSettingsCategories)
			Init();
		
		m_mSettingsCategories[config.Type()] = config;
	}
}

//------------------------------------------------------------------------------------------------
//! Settings for a category
[BaseContainerProps()]
class ACE_SettingsCategoryConfig
{
}
