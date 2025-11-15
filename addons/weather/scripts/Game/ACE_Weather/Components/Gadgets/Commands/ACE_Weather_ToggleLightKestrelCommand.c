//------------------------------------------------------------------------------------------------
//! Used for forwarding button clicks to gadget screen
class ACE_Weather_ToggleLightKestrelCommand : ACE_IGadgetCommand
{
	[Attribute(defvalue: "1973860", uiwidget: UIWidgets.ColorPicker, desc: "Backlight color")]
	protected int m_iBacklightColor;
	
	protected ACE_RenderTargetComponent m_pRTComponent;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity gadget)
	{
		super.Init(gadget);
		m_pRTComponent = ACE_RenderTargetComponent.Cast(gadget.FindComponent(ACE_RenderTargetComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	override void Execute()
	{
		if (m_pRTComponent.GetEnabledBacklightColor() == m_iBacklightColor)
			m_pRTComponent.RequestSetEnabledBacklightColor(Color.BLACK);
		else
			m_pRTComponent.RequestSetEnabledBacklightColor(m_iBacklightColor);
	}
}
