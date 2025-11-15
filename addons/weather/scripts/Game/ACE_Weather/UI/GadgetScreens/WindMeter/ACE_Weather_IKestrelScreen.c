//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_EGadgetScreenID, "m_eID")]
class ACE_Weather_IKestrelScreen : ACE_IGadgetScreenHandler
{ 	
	[Attribute(uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(ACE_EGadgetScreenID))]
	ACE_EGadgetScreenID m_eNextScreenID;
	
	[Attribute(uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(ACE_EGadgetScreenID))]
	ACE_EGadgetScreenID m_ePreviousScreenID;
	
	protected ACE_Weather_KestrelGadgetComponent m_Kestrel;
	
	//------------------------------------------------------------------------------------------------
	override void Init(notnull ACE_ScreenGadgetComponent gadget)
	{
		super.Init(gadget);
		m_Kestrel = ACE_Weather_KestrelGadgetComponent.Cast(gadget);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnOpen(Widget root)
	{
		super.OnOpen(root);
		ACE_WidgetTools.SetDirectChildrenVisible(root.FindAnyWidget("ContentOverlay"), false);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnButtonClickServer(ACE_EGadgetButtonID buttonID)
	{
		switch (buttonID)
		{
			case ACE_EGadgetButtonID.UP:
			{
				m_Kestrel.ChangeScreen(m_ePreviousScreenID);
				return true;
			};
			
			case ACE_EGadgetButtonID.DOWN:
			{
				m_Kestrel.ChangeScreen(m_eNextScreenID);
				return true;
			};
		}
		
		return false;
	}
}
