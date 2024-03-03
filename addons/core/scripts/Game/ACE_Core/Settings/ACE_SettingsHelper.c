//------------------------------------------------------------------------------------------------
//! Helper tool for settings
class ACE_SettingsHelperT<Class T>
{
	//------------------------------------------------------------------------------------------------
	//! Return settings for a mod
	static T GetModSettings()
	{
		ACE_SettingsConfig settings = ArmaReforgerScripted.ACE_GetSettings();
		if (settings)
			return T.Cast(settings.GetModSettings(T));
		
		return null;
	}
}

//------------------------------------------------------------------------------------------------
//! Helper tool for settings
class ACE_SettingsHelper
{
	//------------------------------------------------------------------------------------------------
	//! Overrides settings for a mod
	static void SetModSettings(ACE_ModSettings config)
	{
		ACE_SettingsConfig settings = ArmaReforgerScripted.ACE_GetSettings();
		if (settings)
			settings.SetModSettings(config);
	}
}
