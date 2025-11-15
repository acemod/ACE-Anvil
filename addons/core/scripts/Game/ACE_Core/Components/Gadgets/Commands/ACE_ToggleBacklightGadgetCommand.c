//------------------------------------------------------------------------------------------------
//! Used for forwarding button clicks to gadget screen
class ACE_ToggleBacklightGadgetCommand : ACE_IGadgetCommand
{
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
		m_pRTComponent.RequestToggleBacklight(!m_pRTComponent.IsBacklightActive());
	}
}
