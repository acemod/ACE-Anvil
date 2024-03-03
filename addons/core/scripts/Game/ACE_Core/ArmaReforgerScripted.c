//------------------------------------------------------------------------------------------------
//! Main game instance.
//! Created when the game starts (not when a world starts!) and persists until the game is closed.
modded class ArmaReforgerScripted : ChimeraGame
{
	static protected ref ACE_SettingsConfig m_ACE_Settings;
	
	//------------------------------------------------------------------------------------------------
	//! Load settings when starting the game for the first time
	override bool OnGameStart()
	{
		if (!m_ACE_Settings)
		{
			// Load settings from config file
			m_ACE_Settings = SCR_ConfigHelperT<ACE_SettingsConfig>.GetConfigObject("{A305FEB7400A2965}Configs/ACE/Settings.conf");
			if (!m_ACE_Settings)
				return false;
			
			// Override with settings present in mission header
			SCR_MissionHeader missionHeader = SCR_MissionHeader.Cast(GetGame().GetMissionHeader());
			if (missionHeader)
				missionHeader.ACE_ApplyToSettingConfig(m_ACE_Settings);
		};
		
		return super.OnGameStart();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return all settings
	static ACE_SettingsConfig ACE_GetSettings()
	{
		return m_ACE_Settings;
	}
}
