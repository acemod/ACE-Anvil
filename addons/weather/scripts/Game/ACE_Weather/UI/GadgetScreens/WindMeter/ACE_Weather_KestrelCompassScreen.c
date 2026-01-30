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
		Widget compass = root.FindAnyWidget("Compass");
		compass.SetVisible(true);
		m_wCardinal = TextWidget.Cast(compass.FindAnyWidget("Cardinal"));
		m_wBearing = TextWidget.Cast(compass.FindAnyWidget("Bearing"));
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(float timeSlice)
	{
		int gadgetDir = m_Kestrel.GetDirection();
		m_wCardinal.SetText(ACE_CompassTools.GetCardinalFromBearing(gadgetDir));
		m_wBearing.SetText(string.Format("%1°", gadgetDir.ToString(3)));
	}
}
