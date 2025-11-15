//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(ACE_EGadgetScreenID, "m_eID")]
class ACE_Weather_KestrelProjWindSpeedScreen : ACE_Weather_IKestrelDefaultDataScreen
{
	[Attribute(desc: "Angle of projection in degrees", params: "0 360")]
	protected float m_fProjectionAngle;
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(float timeSlice)
	{
		float projectedSpeed = m_pKestrel.GetMeasuredProjectedWindSpeed(m_fProjectionAngle);
		m_wMainData.SetText(projectedSpeed.ToString(-1, 1));
		float speed = m_pKestrel.GetMeasuredWindSpeed();
		int bearing = m_pKestrel.GetDirection();
		m_wSecondaryData.SetText(string.Format("%1 m/s @ %2°", speed.ToString(-1, 1), bearing.ToString(3)));
	}
	
	//------------------------------------------------------------------------------------------------
	override bool HasSecondaryData()
	{
		return true;
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
				m_pKestrel.PushScreen(ACE_EGadgetScreenID.KESTREL_REF_HEADING_MENU);
				return true;
			}
		}
		
		return false;
	}
}
