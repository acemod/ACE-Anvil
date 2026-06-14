//------------------------------------------------------------------------------------------------
//! Base container for all settings
[BaseContainerProps(configRoot: true)]
class ACE_SettingsConfig
{
	[Attribute(desc: "Definitions of settings for all mods")]
	protected ref array<ref ACE_ModSettings> m_aInitialModSettings;
	
	//------------------------------------------------------------------------------------------------
	//! Return settings for all categories
	array<ref ACE_ModSettings> GetAllModSettings()
	{
		return m_aInitialModSettings;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return settings for a mod
	ACE_ModSettings GetModSettings(typename modSettingsType)
	{
		foreach (ACE_ModSettings entry : m_aInitialModSettings)
		{
			if (entry.Type() == modSettingsType)
				return entry;
		}
		
		return null;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Set settings for a mod
	void SetModSettings(notnull ACE_ModSettings settings)
	{
		for (int i, count = m_aInitialModSettings.Count(); i < count; i++)
		{
			if (m_aInitialModSettings[i].Type() == settings.Type())
			{
				m_aInitialModSettings[i] = settings;
				return;
			}
		}
		
		m_aInitialModSettings.Insert(settings);
	}
}
