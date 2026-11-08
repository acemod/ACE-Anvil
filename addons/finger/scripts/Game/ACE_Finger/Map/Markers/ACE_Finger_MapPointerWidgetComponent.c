//------------------------------------------------------------------------------------------------
//! Map marker layout component
//! Attached to root of marker base layout
class ACE_Finger_MapPointerWidgetComponent : SCR_MapMarkerWidgetComponent
{
	//------------------------------------------------------------------------------------------------
	void ResizeImage(float w, float h)
	{
		vector size = m_wMarkerIcon.GetSize();
		m_wMarkerIcon.SetSize(size[0] * w, size[1] * h);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Vanilla only shows author while mouse is hovering over the marker, but we want it to be permanently
	//! shown when m_bShowAuthor is enabled
	override protected void ShowAuthor(bool state)
	{
		super.ShowAuthor(m_bShowAuthor);
	}
}
