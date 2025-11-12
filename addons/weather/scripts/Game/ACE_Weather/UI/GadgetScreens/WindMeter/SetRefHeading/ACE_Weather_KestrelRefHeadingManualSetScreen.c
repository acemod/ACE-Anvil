//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_EGadgetScreenID, "m_eID")]
class ACE_Weather_KestrelRefHeadingManualSetScreen : ACE_Weather_IKestrelRefHeadingSetScreen
{
	//------------------------------------------------------------------------------------------------
	//! Copy ref heading to tmp
	override void OnActivateServer()
	{
		m_pKestrel.SetTmpRefHeading(m_pKestrel.GetRefHeading());
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnRefreshScreen()
	{
		if (m_wHeading)
			m_wHeading.SetText(string.Format("%1°", m_pKestrel.GetTmpRefHeading().ToString(3)));
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnButtonClickServer(ACE_EGadgetButtonID buttonID)
	{
		if (super.OnButtonClickServer(buttonID))
			return true;
		
		switch (buttonID)
		{
			case ACE_EGadgetButtonID.LEFT:
			{
				m_pKestrel.SetTmpRefHeading((m_pKestrel.GetTmpRefHeading() - 1) % 360);
				return true;
			}
			
			case ACE_EGadgetButtonID.RIGHT:
			{
				m_pKestrel.SetTmpRefHeading((m_pKestrel.GetTmpRefHeading() + 1) % 360);
				return true;
			}
			
			case ACE_EGadgetButtonID.ENTER:
			{
				m_pKestrel.SetRefHeading(m_pKestrel.GetTmpRefHeading());
				m_pKestrel.PopToRootScreen();
				return true;
			}
		}
		
		return false;
	}
}
