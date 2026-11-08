//------------------------------------------------------------------------------------------------
//! Add mod settings to mission header
[BaseContainerProps()]
modded class ACE_MissionHeaderSettings
{
	[Attribute()]
	protected ref ACE_Explosives_Settings m_ACE_Explosives_Settings;
	
	//------------------------------------------------------------------------------------------------
	//! Applies settings from mission header to config
	override void ApplyToSettingsConfig(notnull ACE_SettingsConfig config)
	{
		super.ApplyToSettingsConfig(config);
		
		if (m_ACE_Explosives_Settings)
			config.SetModSettings(m_ACE_Explosives_Settings);
	}
}

