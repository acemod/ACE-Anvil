//------------------------------------------------------------------------------------------------
class ACE_GamepadPhysicalButtonUIComponent : ScriptedWidgetComponent
{
	protected Widget m_wWidget;
	protected ACE_PhysicalButtonsComponent m_PhysicalButtonsComponent;
	protected ACE_PhysicalButtonConfig m_ButtonConfig;
	
	protected static const int BUTTON_RELEASE_DELAY_MS = 250;
	
	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		m_wWidget = w;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetPhysicalButton(ACE_PhysicalButtonsComponent component, ACE_PhysicalButtonConfig buttonConfig)
	{
		m_PhysicalButtonsComponent = component;
		m_ButtonConfig = buttonConfig;
		m_wWidget.SetName(buttonConfig.m_sColliderName);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnClick(Widget w, int x, int y, int button)
	{
		m_PhysicalButtonsComponent.SetButtonState(m_ButtonConfig.m_sColliderName, true);
		GetGame().GetCallqueue().CallLater(m_PhysicalButtonsComponent.SetButtonState, BUTTON_RELEASE_DELAY_MS, false, m_ButtonConfig.m_sColliderName, false);
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
