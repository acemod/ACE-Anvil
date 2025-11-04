//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_EGadgetScreenID, "m_eID")]
class ACE_Weather_KestrelRefHeadingAutoSetScreen : ACE_Weather_IKestrelRefHeadingSetScreen
{	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(float timeSlice)
	{
		int heading = Math.Round(m_pKestrel.GetDirection());
		m_wHeading.SetText(string.Format("%1°", heading.ToString(3)));
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnButtonClickServer(ACE_EGadgetButtonID buttonID)
	{
		if (super.OnButtonClickServer(buttonID))
			return true;
		
		switch (buttonID)
		{
			case ACE_EGadgetButtonID.ENTER:
			{
				m_pKestrel.SetRefHeading(Math.Round(m_pKestrel.GetDirection()));
				m_pKestrel.PopToRootScreen();
				return true;
			};
		}
		
		return false;
	}
}
