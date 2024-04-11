//------------------------------------------------------------------------------------------------
//! Helper tool for settings
class ACE_SettingsHelperT<Class T>
{
    //------------------------------------------------------------------------------------------------
    //! Return settings for a mod
    static T GetModSettings()
    {
	ACE_SettingsConfig config = ArmaReforgerScripted.ACE_GetSettingsConfig();
	if (config)
	    return T.Cast(config.GetModSettings(T));

	return null;
    }
}

//------------------------------------------------------------------------------------------------
//! Helper tool for settings
class ACE_SettingsHelper
{
    //------------------------------------------------------------------------------------------------
    //! Overrides settings for a mod
    static void SetModSettings(notnull ACE_ModSettings settings)
    {
	ACE_SettingsConfig config = ArmaReforgerScripted.ACE_GetSettingsConfig();
	if (config)
	    config.SetModSettings(settings);
    }
}
