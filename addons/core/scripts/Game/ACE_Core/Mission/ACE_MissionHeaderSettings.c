//------------------------------------------------------------------------------------------------
//! Mod this class to add setting for your mod
[BaseContainerProps()]
class ACE_MissionHeaderSettings
{
	//------------------------------------------------------------------------------------------------
	//! Override this method to add the mod settings to the config
	//! with ACE_SettingsConfig.SetSettingsCategoryConfig
	void ApplyToSettings(ACE_SettingsConfig config)
	{
	}
}

//------------------------------------------------------------------------------------------------
//! Add mod settings to mission header
[BaseContainerProps()]
modded class ACE_MissionHeaderSettings
{
	[Attribute()]
	ref ACE_Medical_SettingsConfig m_ACE_Medical_Settings;
	
	//------------------------------------------------------------------------------------------------
	//! Applies settings from mission header to config
	override void ApplyToSettings(ACE_SettingsConfig config)
	{
		if (m_ACE_Medical_Settings)
			config.SetSettingsCategoryConfig(m_ACE_Medical_Settings);
	}
}
