//------------------------------------------------------------------------------------------------
//! Used for forwarding button clicks to gadget screen
class ACE_ToggleBacklightGadgetCommand : ACE_IGadgetCommand
{
	protected ACE_ScreenGadgetComponent m_pScreenComponent;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity gadget)
	{
		super.Init(gadget);
		m_pScreenComponent = ACE_ScreenGadgetComponent.Cast(gadget.FindComponent(ACE_ScreenGadgetComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	override void Execute()
	{
		m_pScreenComponent.RequestToggleBacklight(!m_pScreenComponent.IsBacklightActive());
	}
}
