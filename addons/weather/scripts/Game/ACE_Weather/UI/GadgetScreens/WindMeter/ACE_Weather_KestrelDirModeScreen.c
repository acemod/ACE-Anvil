//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_EGadgetScreenID, "m_eID")]
class ACE_Weather_KestrelDirModeScreen : ACE_Weather_IKestrelScreen
{
	protected TextWidget m_wMode;
	
	//------------------------------------------------------------------------------------------------
	override void OnOpen(Widget root)
	{
		super.OnOpen(root);
		Widget display = root.FindAnyWidget("DisplayDirMode");
		display.SetVisible(true);
		Widget modeRow = display.FindAnyWidget("ModeRow");
		m_wMode = TextWidget.Cast(modeRow.FindAnyWidget("Value"));
		Widget variationRow = display.FindAnyWidget("VariationRow");
		TextWidget variation = TextWidget.Cast(variationRow.FindAnyWidget("Value"));
		variation.SetText(m_Kestrel.GetDeclination().ToString(lenDec: 1));
		
		m_Kestrel.SetTmpUseTrueNorth(m_Kestrel.GetUseTrueNorth());
		OnRefreshScreen();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnRefreshScreen()
	{
		if (!m_wMode)
			return;
		
		if (m_Kestrel.GetTmpUseTrueNorth())
		{
			m_wMode.SetText("True");
		}
		else
		{
			m_wMode.SetText("Magnetic");
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnButtonClickServer(ACE_EGadgetButtonID buttonID)
	{
		if (super.OnButtonClickServer(buttonID))
			return true;
		
		switch (buttonID)
		{
			case ACE_EGadgetButtonID.LEFT:
			case ACE_EGadgetButtonID.RIGHT:
			{
				m_Kestrel.SetTmpUseTrueNorth(!m_Kestrel.GetTmpUseTrueNorth());
				return true;
			}
			
			case ACE_EGadgetButtonID.ENTER:
			{
				m_Kestrel.SetUseTrueNorth(m_Kestrel.GetTmpUseTrueNorth());
				m_Kestrel.PopScreen();
				return true;
			}
			
			case ACE_EGadgetButtonID.POWER:
			{
				m_Kestrel.PopScreen();
				return true;
			}
		}
		
		return false;
	}
}
 	