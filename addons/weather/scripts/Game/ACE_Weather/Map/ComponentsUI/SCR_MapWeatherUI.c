//------------------------------------------------------------------------------------------------
modded class SCR_MapWeatherUI : SCR_MapUIBaseComponent
{
	//------------------------------------------------------------------------------------------------
	override protected bool CreateLayout()
	{
		if (!super.CreateLayout())
			return false;
		
		ACE_Weather_Settings settings = ACE_SettingsHelperT<ACE_Weather_Settings>.GetModSettings();
		if (settings)
			m_iDataUpdateCooldown = settings.m_iMapWindReportInterval;
		
		// Disable report if negative
		if (m_iDataUpdateCooldown < 0)
		{
			Widget w = m_wWeatherDataRoot.FindAnyWidget("Wind");
			if (w)
				w.SetVisible(false);
		}
		
		return true;
	}
}
