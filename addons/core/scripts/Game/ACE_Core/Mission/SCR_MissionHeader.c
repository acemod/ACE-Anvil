//------------------------------------------------------------------------------------------------
//! Add ACE settings to mission header
modded class SCR_MissionHeader : MissionHeader
{
	[Attribute(desc: "Settings for all ACE mods")]
	protected ref ACE_MissionHeaderSettings m_ACE_Settings;
	
	//------------------------------------------------------------------------------------------------
	//! Applies settings from mission header to config
	void ACE_ApplyToSettingsConfig(ACE_SettingsConfig config)
	{
		if (m_ACE_Settings)
			m_ACE_Settings.ApplyToSettingsConfig(config);
	}
}
