//------------------------------------------------------------------------------------------------
class ACE_SettingsSystem : GameSystem
{
	static protected ref ACE_SettingsConfig s_CachedSettingsConfig;
	
	[RplProp()]
	protected ref ACE_SettingsConfig m_SettingsConfig;
	
	//------------------------------------------------------------------------------------------------
	static ACE_SettingsSystem GetInstance()
	{
		ChimeraWorld world = GetGame().GetWorld();
		return ACE_SettingsSystem.Cast(world.FindSystem(ACE_SettingsSystem));
	}
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(false)
			.SetUnique(true)
			.SetLocation(WorldSystemLocation.Both);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Override with settings present in mission header
	//! Only done the first time due to subsequent mission headers not containing server config settings
	//! See https://feedback.bistudio.com/T172515
	void ACE_SettingsSystem()
	{
		if (!s_CachedSettingsConfig)
		{
			s_CachedSettingsConfig = SCR_ConfigHelperT<ACE_SettingsConfig>.GetConfigObject("{A305FEB7400A2965}Configs/ACE/Settings.conf");
			
			SCR_MissionHeader mission = SCR_MissionHeader.Cast(GetGame().GetMissionHeader());
			if (mission)
				mission.ACE_ApplyToSettingsConfig(s_CachedSettingsConfig);
		}

		m_SettingsConfig = s_CachedSettingsConfig;
	}

	//------------------------------------------------------------------------------------------------
	//! Set settings for a mod
	void SetModSettings(notnull ACE_ModSettings settings)
	{
		m_SettingsConfig.SetModSettings(settings);
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return settings for a mod
	ACE_ModSettings GetModSettings(typename modSettingsType)
	{
		return m_SettingsConfig.GetModSettings(modSettingsType);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return settings for all categories
	array<ref ACE_ModSettings> GetAllModSettings()
	{
		return m_SettingsConfig.GetAllModSettings();
	}
}
