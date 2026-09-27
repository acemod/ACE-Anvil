//------------------------------------------------------------------------------------------------
//! Helper tool for settings
class ACE_SettingsHelperT<Class T>
{
	//------------------------------------------------------------------------------------------------
	//! Return settings for a mod
	static T GetModSettings()
	{
		ACE_SettingsSystem system = ACE_SettingsSystem.GetInstance();
		if (system)
			return T.Cast(system.GetModSettings(T));
		
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
		ACE_SettingsSystem system = ACE_SettingsSystem.GetInstance();
		if (system)
			system.SetModSettings(settings);
	}
}
