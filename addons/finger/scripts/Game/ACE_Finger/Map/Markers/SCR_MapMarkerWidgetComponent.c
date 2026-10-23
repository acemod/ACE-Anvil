//! Map marker layout component
//! Attached to root of marker base layout
modded class SCR_MapMarkerWidgetComponent : SCR_ScriptedWidgetComponent
{
	void ACE_ResizeImage(float w, float h)
	{
		vector size = m_wMarkerIcon.GetSize();
		m_wMarkerIcon.SetSize(size[0] * w, size[1] * h);
	}
}
