//------------------------------------------------------------------------------------------------
//! Add mod settings to mission header
[BaseContainerProps()]
modded class ACE_MissionHeaderSettings
{
	[Attribute()]
	protected ref ACE_Finger_Settings m_ACE_Finger_Settings;
	
	//------------------------------------------------------------------------------------------------
	//! Applies settings from mission header to config
	override void ApplyToSettingsConfig(notnull ACE_SettingsConfig config)
	{
		if (m_ACE_Finger_Settings)
			config.SetModSettings(m_ACE_Finger_Settings);
	}
}
