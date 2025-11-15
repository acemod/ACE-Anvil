//------------------------------------------------------------------------------------------------
//! Used for forwarding button clicks to gadget screen
class ACE_ScreenButtonClickGadgetCommand : ACE_IGadgetCommand
{
	[Attribute(desc: "ID of the screen button", uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(ACE_EGadgetButtonID))]
	protected ACE_EGadgetButtonID m_eButtonID;
	
	protected ACE_ScreenGadgetComponent m_ScreenManager;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity gadget)
	{
		super.Init(gadget);
		m_ScreenManager = ACE_ScreenGadgetComponent.Cast(m_Gadget.FindComponent(ACE_ScreenGadgetComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	override void Execute()
	{
		m_ScreenManager.OnButtonClick(m_eButtonID);
	}
}
