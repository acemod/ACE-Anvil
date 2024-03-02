//------------------------------------------------------------------------------------------------
//! Add mod settings to mission header
[BaseContainerProps()]
modded class ACE_MissionHeaderSettings
{
	[Attribute()]
	ref ACE_Medical_Settings m_ACE_Medical_Settings;
	
	//------------------------------------------------------------------------------------------------
	//! Applies settings from mission header to config
	override void ApplyToSettings(ACE_SettingsConfig config)
	{
		if (m_ACE_Medical_Settings)
			config.SetSettingsCategoryConfig(m_ACE_Medical_Settings);
	}
}
