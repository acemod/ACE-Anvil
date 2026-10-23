//------------------------------------------------------------------------------------------------
class ACE_Inventory_SettingsSubMenu : SCR_SettingsSubMenuBase
{
	//------------------------------------------------------------------------------------------------
	override void OnTabCreate(Widget menuRoot, ResourceName buttonsLayout, int index)
	{
		super.OnTabCreate(menuRoot, buttonsLayout, index);
		m_aSettingsBindings.Insert(new ACE_Inventory_SettingsBinding("ACE_Inventory_DisplaySettings", "m_bShowDimensions", "ACE_Inventory_ShowDimensions"));
		m_aSettingsBindings.Insert(new ACE_Inventory_SettingsBinding("ACE_Inventory_DisplaySettings", "m_bShowCompartmentCount", "ACE_Inventory_ShowCompartmentCount"));
		m_aSettingsBindings.Insert(new ACE_Inventory_SettingsBinding("ACE_Inventory_DisplaySettings", "m_bShowParenthesizedValues", "ACE_Inventory_ShowParenthesizedValues"));
		LoadSettings();
	}

	//------------------------------------------------------------------------------------------------
	override void OnTabShow()
	{
		super.OnTabShow();
		LoadSettings(true, false);
	}

	//------------------------------------------------------------------------------------------------
	override void OnTabHide()
	{
		super.OnTabHide();
		ACE_Inventory_DisplaySettings.s_OnChanged.Invoke();
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnMenuItemChanged(SCR_SettingsBindingBase binding)
	{
		super.OnMenuItemChanged(binding);
		ACE_Inventory_DisplaySettings.s_OnChanged.Invoke();
	}
}
