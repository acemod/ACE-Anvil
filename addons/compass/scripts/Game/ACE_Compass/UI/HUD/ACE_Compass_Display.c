//------------------------------------------------------------------------------------------------
class ACE_Compass_Display : SCR_InfoDisplayExtended
{
	protected TextWidget m_wBearing;
	protected TextWidget m_wCardinal;
	
	//------------------------------------------------------------------------------------------------
	override void DisplayStartDraw(IEntity owner)
	{
		super.DisplayStartDraw(owner);
		m_wBearing = TextWidget.Cast(m_wRoot.FindAnyWidget("bearing"));
		m_wCardinal = TextWidget.Cast(m_wRoot.FindAnyWidget("cardinal"));
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void DisplayUpdate(IEntity owner, float timeSlice)
	{
		super.DisplayUpdate(owner, timeSlice);
		
		if (!m_bShown)
			return;
		
		vector transform[4];
		GetGame().GetWorld().GetCurrentCamera(transform);
		int bearing = Math.Round(ACE_CompassTools.GetBearingFromTransform(transform));
		m_wBearing.SetText(ACE_StringTools.Zfill(bearing.ToString(), 3));
		m_wCardinal.SetText(ACE_CompassTools.GetCardinalFromBearing(bearing));
	}
}
