//------------------------------------------------------------------------------------------------
//! Helper tool for settings
class ACE_SettingsHelper
{
	static SCR_BaseGameMode m_pGameMode;
	
	//------------------------------------------------------------------------------------------------
	//! Return a setting
	static ACE_Setting Get(ACE_ESettings settingId)
	{
		if (!m_pGameMode)
			m_pGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		
		return m_pGameMode.ACE_GetSetting(settingId);
	}
	
	//------------------------------------------------------------------------------------------------
	static array<ACE_Setting> GetAll()
	{
		return m_pGameMode.ACE_GetAllSettings();
	}
}
