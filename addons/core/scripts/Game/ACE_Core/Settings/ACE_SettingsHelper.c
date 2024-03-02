//------------------------------------------------------------------------------------------------
//! Helper tool for settings
class ACE_SettingsHelperT<Class T>
{
	//------------------------------------------------------------------------------------------------
	//! Return settings for a category
	static T GetCategorySettings()
	{
		ACE_SettingsConfig settings = ArmaReforgerScripted.ACE_GetSettings();
		if (settings)
			return T.Cast(settings.GetSettingsCategoryConfig(T));
		
		return null;
	}
}

//------------------------------------------------------------------------------------------------
//! Helper tool for settings
class ACE_SettingsHelper
{
	//------------------------------------------------------------------------------------------------
	//! Overrides settings for a category
	static void SetCategorySettings(ACE_SettingsCategoryConfig config)
	{
		ACE_SettingsConfig settings = ArmaReforgerScripted.ACE_GetSettings();
		if (settings)
			settings.SetSettingsCategoryConfig(config);
	}
}
