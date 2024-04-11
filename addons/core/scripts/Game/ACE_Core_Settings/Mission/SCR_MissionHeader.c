/*
//------------------------------------------------------------------------------------------------
//! Add ACE settings to mission header
modded class SCR_MissionHeader : MissionHeader
{
	[Attribute(desc: "Settings for all ACE mods")]
	protected ref array<ref ACE_MissionHeaderSettingConfigEntry> m_ACE_Settings;
	
	//------------------------------------------------------------------------------------------------
	//! Applies settings from mission header to config
	void ACE_ApplyToSettingsConfig(notnull ACE_SettingsConfig config)
	{
		if (m_ACE_Settings)
			m_ACE_Settings.ApplyToSettingsConfig(config);
	}
}

//------------------------------------------------------------------------------------------------ 
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_ESettings, "m_eSettingId")]
class ACE_MissionHeaderSettingConfigEntry : ACE_BaseSettingConfigEntry
{
	// -- TO DO: Check if needed. Maybe mission header can already set enum from strings
	[Attribute()]
	protected string m_sSettingName;
	
	[Attribute()]
	protected int m_iPriority;
	
	[Attribute()]
	protected float m_Value;
	
	//------------------------------------------------------------------------------------------------ 
	override ACE_ESettings GetId()
	{
		if (m_sSettingName)
			return typename.StringToEnum(ACE_ESettings, m_sSettingName);
		
		return super.GetId();
	}
	
	//------------------------------------------------------------------------------------------------ 
	int GetPriority() { return m_iPriority; }
	
	//------------------------------------------------------------------------------------------------ 
	ACE_BaseVar GetValue() { return ACE_BaseVar.CreateFloat(m_Value); }
}
*/