//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_EGadgetScreenID, "m_eID")]
class ACE_IGadgetScreenHandler
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(ACE_EGadgetScreenID))]
	ACE_EGadgetScreenID m_eID;
	
	protected ACE_ScreenGadgetComponent m_pGadget;
	protected Widget m_wRoot;
	
	//------------------------------------------------------------------------------------------------
	void Init(notnull ACE_ScreenGadgetComponent gadget)
	{
		m_pGadget = gadget;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnOpen(Widget root)
	{
		m_wRoot = root;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnClose(Widget root);
	
	//------------------------------------------------------------------------------------------------
	void OnUpdate(float timeSlice);
	
	//------------------------------------------------------------------------------------------------
	void OnButtonClick(ACE_EGadgetButtonID buttonID);
	
	//------------------------------------------------------------------------------------------------
	protected void HideDirectChildren(Widget w)
	{
		Widget child = w.GetChildren();
		while (child)
		{
			child.SetVisible(false);
			child = child.GetSibling();
		}
	}
}
