//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_EGadgetScreenID, "m_eID")]
class ACE_Weather_KestrelCompassScreen : ACE_Weather_IKestrelDataScreen
{
	protected TextWidget m_wCardinal;
	protected TextWidget m_wBearing;
	
	//------------------------------------------------------------------------------------------------
	override void OnOpen(Widget root)
	{
		super.OnOpen(root);
		Widget data = root.FindAnyWidget("DisplayData");
		Widget compass = data.FindAnyWidget("Compass");
		compass.SetVisible(true);
		m_wCardinal = TextWidget.Cast(compass.FindAnyWidget("Cardinal"));
		m_wBearing = TextWidget.Cast(compass.FindAnyWidget("Bearing"));
		
		RichTextWidget footer = RichTextWidget.Cast(data.FindAnyWidget("Footer"));
		if (m_Kestrel.GetUseTrueNorth())
		{
			string signPostfix = "E";
			
			float declination = m_Kestrel.GetDeclination();
			if (declination < 0)
			{
				signPostfix = "W";
				declination = -declination;
			}
			
			footer.SetTextFormat("True N %1 %2\n— set mode", declination.ToString(lenDec: 1), signPostfix);
		}
		else
		{
			footer.SetText("Magnetic North\n— set mode");
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(float timeSlice)
	{
		int gadgetDir = Math.Round(m_Kestrel.GetDirection());
		m_wCardinal.SetText(ACE_CompassTools.GetCardinalFromBearing(gadgetDir));
		m_wBearing.SetText(string.Format("%1°", gadgetDir.ToString(3)));
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
				m_Kestrel.PushScreen(ACE_EGadgetScreenID.KESTREL_DIR_MODE_SET);
				return true;
			}
		}
		
		return false;
	}
}
