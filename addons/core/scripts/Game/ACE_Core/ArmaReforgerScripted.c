//------------------------------------------------------------------------------------------------
//! Main game instance.
//! Created when the game starts (not when a world starts!) and persists until the game is closed.
modded class ArmaReforgerScripted : ChimeraGame
{
	static protected ref ACE_SettingsConfig s_ACE_SettingsConfig;
	static protected bool s_bACE_WasMissionHeaderApplied = false;
	
	//------------------------------------------------------------------------------------------------
	//! Load settings from config file
	override void OnAfterInit(BaseWorld world)
	{
		super.OnAfterInit(world);
		
		if (Replication.IsServer() && !s_ACE_SettingsConfig)
			s_ACE_SettingsConfig = SCR_ConfigHelperT<ACE_SettingsConfig>.GetConfigObject("{A305FEB7400A2965}Configs/ACE/Settings.conf");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Override with settings present in mission header
	//! Only done the first time due to subsequent mission headers not containing server config settings
	//! See https://feedback.bistudio.com/T172515
	override protected void OnMissionSet(MissionHeader mission)
	{
		super.OnMissionSet(mission);
		
		if (!Replication.IsServer() || s_bACE_WasMissionHeaderApplied)
			return;
		
		SCR_MissionHeader scriptedMission = SCR_MissionHeader.Cast(mission);
		if (!scriptedMission)
			return;
		
		scriptedMission.ACE_ApplyToSettingsConfig(ACE_GetSettingsConfig());
		s_bACE_WasMissionHeaderApplied = true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return all settings
	static ACE_SettingsConfig ACE_GetSettingsConfig()
	{
		return s_ACE_SettingsConfig;
	}
}
