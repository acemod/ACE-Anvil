//------------------------------------------------------------------------------------------------
class ACE_Inventory_DisplaySettings : ModuleGameSettings
{
	[Attribute("1", desc: "Show item and pocket dimensions in inventory tooltips and headers.")]
	bool m_bShowDimensions;

	[Attribute("1", desc: "Show compartment counts in inventory tooltips and headers.")]
	bool m_bShowCompartmentCount;

	[Attribute("1", desc: "Show parenthesized single-pocket volume and dimensions in inventory headers.")]
	bool m_bShowParenthesizedValues;

	static ref ScriptInvoker s_OnChanged = new ScriptInvoker();
	protected static bool s_bReportedFailure;

	//------------------------------------------------------------------------------------------------
	static BaseContainer GetInstance()
	{
		UserSettings userSettings = GetGame().GetGameUserSettings();
		BaseContainer settings;
		if (userSettings)
			settings = userSettings.GetModule("ACE_Inventory_DisplaySettings");
		if (!settings)
			ReportFailure();
		return settings;
	}

	//------------------------------------------------------------------------------------------------
	protected static void ReportFailure()
	{
		if (s_bReportedFailure)
			return;
		Print("[ACE Inventory] Metrics: Could not access player display settings. Check the settings module and bindings.", LogLevel.ERROR);
		s_bReportedFailure = true;
	}

	//------------------------------------------------------------------------------------------------
	protected static bool Read(string property)
	{
		BaseContainer settings = GetInstance();
		bool value = true;
		if (settings && !settings.Get(property, value))
			ReportFailure();
		return value;
	}

	//------------------------------------------------------------------------------------------------
	static bool ShowDimensions()
	{
		return Read("m_bShowDimensions");
	}

	//------------------------------------------------------------------------------------------------
	static bool ShowCompartmentCount()
	{
		return Read("m_bShowCompartmentCount");
	}

	//------------------------------------------------------------------------------------------------
	static bool ShowParenthesizedValues()
	{
		return Read("m_bShowParenthesizedValues");
	}
}
