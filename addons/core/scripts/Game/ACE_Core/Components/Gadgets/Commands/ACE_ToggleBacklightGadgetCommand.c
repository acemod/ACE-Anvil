//------------------------------------------------------------------------------------------------
//! Used for toggling screen backlight
class ACE_ToggleBacklightGadgetCommand : ACE_IGadgetCommand
{
	protected ACE_ScreenGadgetComponent m_ScreenComponent;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity gadget)
	{
		super.Init(gadget);
		m_ScreenComponent = ACE_ScreenGadgetComponent.Cast(gadget.FindComponent(ACE_ScreenGadgetComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	override void Execute()
	{
		m_ScreenComponent.RequestToggleBacklight(!m_ScreenComponent.IsBacklightActive());
	}
}
