//------------------------------------------------------------------------------------------------
//! Used for forwarding button clicks to gadget screen
class ACE_Weather_ToggleLightKestrelCommand : ACE_IGadgetCommand
{
	[Attribute(defvalue: "13172680", uiwidget: UIWidgets.ColorPicker, desc: "Primary backlight color")]
	protected int m_iPrimaryBacklightColor;
	
	[Attribute(defvalue: "1973860", uiwidget: UIWidgets.ColorPicker, desc: "Secondary backlight color that can be toggled to")]
	protected int m_iSecondaryBacklightColor;
	
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
		if (m_pRTComponent.GetEnabledBacklightColor() == m_iPrimaryBacklightColor)
			m_pRTComponent.RequestSetEnabledBacklightColor(m_iSecondaryBacklightColor);
		else
			m_pRTComponent.RequestSetEnabledBacklightColor(m_iPrimaryBacklightColor);
	}
}
