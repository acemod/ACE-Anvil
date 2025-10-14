//------------------------------------------------------------------------------------------------
class ACE_GamepadPhysicalButtonUIComponent : ScriptedWidgetComponent
{
	protected Widget m_wWidget;
	protected ACE_PhysicalButtonsComponent m_pPhysicalButtonsComponent;
	protected ACE_PhysicalButtonConfig m_pButtonConfig;
	
	protected static const int BUTTON_RELEASE_DELAY_MS = 250;
	
	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		m_wWidget = w;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetPhysicalButton(ACE_PhysicalButtonsComponent component, ACE_PhysicalButtonConfig buttonConfig)
	{
		m_pPhysicalButtonsComponent = component;
		m_pButtonConfig = buttonConfig;
		m_wWidget.SetName(buttonConfig.m_sColliderName);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnClick(Widget w, int x, int y, int button)
	{
		m_pPhysicalButtonsComponent.SetButtonState(m_pButtonConfig.m_sColliderName, true);
		GetGame().GetCallqueue().CallLater(m_pPhysicalButtonsComponent.SetButtonState, BUTTON_RELEASE_DELAY_MS, false, m_pButtonConfig.m_sColliderName, false);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnFocus(Widget w, int x, int y)
	{
		m_wWidget.SetOpacity(1);
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnFocusLost(Widget w, int x, int y)
	{
		m_wWidget.SetOpacity(0);
		return false;
	}
}
