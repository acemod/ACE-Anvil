//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_EGadgetScreenID, "m_eID")]
class ACE_IGadgetScreenHandler
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(ACE_EGadgetScreenID))]
	ACE_EGadgetScreenID m_eID;
	
	//------------------------------------------------------------------------------------------------
	void Init(notnull ACE_ScreenGadgetComponent gadget);
	
	//------------------------------------------------------------------------------------------------
	void OnOpen(Widget root);
	
	//------------------------------------------------------------------------------------------------
	void OnClose(Widget root);
	
	//------------------------------------------------------------------------------------------------
	//! Called when handler becomes active on server (called before OnOpen)
	void OnActivateServer();
	
	//------------------------------------------------------------------------------------------------
	//! Called when handler becomes inactive on server (called before OnClose)
	void OnDeactivateServer();
	
	//------------------------------------------------------------------------------------------------
	void OnUpdate(float timeSlice);
	
	//------------------------------------------------------------------------------------------------
	//! Called when some screen-relevant data on ACE_ScreenGadgetComponent got updated
	void OnRefreshScreen();
	
	//------------------------------------------------------------------------------------------------
	//! Returns true if button got handled (used for optimizing overrides)
	bool OnButtonClickServer(ACE_EGadgetButtonID buttonID);
}
