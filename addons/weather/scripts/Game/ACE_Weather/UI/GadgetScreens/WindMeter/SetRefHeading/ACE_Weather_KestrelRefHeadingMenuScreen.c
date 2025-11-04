//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_EGadgetScreenID, "m_eID")]
class ACE_Weather_KestrelRefHeadingMenuScreen : ACE_Weather_IKestrelRefHeadingScreen
{
	protected TextWidget m_wAuto;
	protected TextWidget m_wManual;
	
	//------------------------------------------------------------------------------------------------
	override void OnOpen(Widget root)
	{
		super.OnOpen(root);
		m_wInstructions.SetVisible(false);
		m_wOptions.SetVisible(true);
		m_wAuto = TextWidget.Cast(m_wOptions.FindAnyWidget("Auto"));
		m_wManual = TextWidget.Cast(m_wOptions.FindAnyWidget("Manual"));
		OnRefreshScreen();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnActivateServer()
	{
		m_pKestrel.SetRefHeadingSetMode(ACE_EGadgetScreenID.KESTREL_REF_HEADING_AUTO_SET);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnRefreshScreen()
	{
		if (!m_wOptions)
			return;
		
		if (m_pKestrel.GetRefHeadingSetMode() == ACE_EGadgetScreenID.KESTREL_REF_HEADING_AUTO_SET)
		{
			m_wAuto.SetColorInt(Color.GRAY);
			m_wManual.SetColorInt(Color.WHITE);
		}
		else
		{
			m_wAuto.SetColorInt(Color.WHITE);
			m_wManual.SetColorInt(Color.GRAY);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnButtonClickServer(ACE_EGadgetButtonID buttonID)
	{
		bool isHandled = false;
		
		switch (buttonID)
		{
			case ACE_EGadgetButtonID.UP:
			{
				m_pKestrel.ToggleRefHeadingSetMode();
				isHandled = true;
				break;
			}
			
			case ACE_EGadgetButtonID.DOWN:
			{
				m_pKestrel.ToggleRefHeadingSetMode();
				isHandled = true;
				break;
			}
			
			case ACE_EGadgetButtonID.ENTER:
			{
				m_pKestrel.PushScreen(m_pKestrel.GetRefHeadingSetMode());
				isHandled = true;
				break;
			}
		}
		
		if (!isHandled)
			return super.OnButtonClickServer(buttonID);
		
		return true;
	}
}
