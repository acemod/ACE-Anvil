//------------------------------------------------------------------------------------------------
class ACE_Inventory_SettingsBinding : SCR_SettingsBindingBase
{
	//------------------------------------------------------------------------------------------------
	void ACE_Inventory_SettingsBinding(string module, string name, string widgetName, string subModule = string.Empty)
	{
		m_bUseGameUserSettings = true;
	}
}
