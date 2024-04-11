//------------------------------------------------------------------------------------------------
//! Base container for all settings
[BaseContainerProps(configRoot: true)]
class ACE_SettingsConfig
{
    [Attribute(desc: "Definitions of settings for all mods")]
    protected ref array<ref ACE_ModSettings> m_aInitialModSettings;
    protected ref map<typename, ref ACE_ModSettings> m_mModSettingsMap;

    //------------------------------------------------------------------------------------------------
    //! Construct map for faster look-up of settings
    //! We can't use maps as attribute, so we initialize the settings with an array
    protected void InitMap()
    {
	m_mModSettingsMap = new map<typename, ref ACE_ModSettings>();

	foreach (ACE_ModSettings settings: m_aInitialModSettings) { m_mModSettingsMap[settings.Type()] = settings; }

	m_aInitialModSettings = null;
    }

    //------------------------------------------------------------------------------------------------
    //! Return settings for all categories
    array<ACE_ModSettings> GetAllModSettings()
    {
	if (!m_mModSettingsMap)
	    InitMap();

	array<ACE_ModSettings> output = {};

	foreach (ACE_ModSettings settings: m_mModSettingsMap) { output.Insert(settings); }

	return output;
    }

    //------------------------------------------------------------------------------------------------
    //! Return settings for a mod
    ACE_ModSettings GetModSettings(typename modSettingsType)
    {
	if (!m_mModSettingsMap)
	    InitMap();

	ACE_ModSettings settings;
	m_mModSettingsMap.Find(modSettingsType, settings);
	return settings;
    }

    //------------------------------------------------------------------------------------------------
    //! Set settings for a mod
    void SetModSettings(notnull ACE_ModSettings settings)
    {
	if (!m_mModSettingsMap)
	    InitMap();

	m_mModSettingsMap[settings.Type()] = settings;
    }
}
