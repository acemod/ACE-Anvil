//------------------------------------------------------------------------------------------------
//! Add mod settings to mission header
[BaseContainerProps()]
modded class ACE_MissionHeaderSettings
{
	[Attribute()]
	protected ref ACE_CBRN_Settings m_ACE_CBRN;
	
	//------------------------------------------------------------------------------------------------
	//! Applies settings from mission header to config
	override void ApplyToSettingsConfig(notnull ACE_SettingsConfig config)
	{
		super.ApplyToSettingsConfig(config);
		
		if (m_ACE_CBRN)
			config.SetModSettings(m_ACE_CBRN);
	}
}
