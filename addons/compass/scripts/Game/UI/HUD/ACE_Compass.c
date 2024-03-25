
class ACE_Compass : SCR_InfoDisplay
{
	const vector SCREEN_POS_OFFSET = "-50 0 0";
	
	protected IEntity m_CompassEntity;
	protected BaseWorld m_World;
	protected WorkspaceWidget m_Workspace;

	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);

		m_World = GetGame().GetWorld();
		
		GetGame().OnUserSettingsChangedInvoker().Insert(OnUserSettingChanged);
		OnUserSettingChanged();
	}

	override protected void UpdateValues(IEntity owner, float timeSlice)
	{
		super.UpdateValues(owner, timeSlice);
		
		if (!m_bShown || !m_CompassEntity)
		{
			return;
		}

		vector worldPos = m_CompassEntity.GetOrigin();
		vector screenPos = m_Workspace.ProjWorldToScreen(worldPos, m_World);
		screenPos += SCREEN_POS_OFFSET;
		
		// Print("compass");
		
		FrameSlot.SetPos(m_wRoot.FindWidget("compassFrame"), screenPos[0], screenPos[1]);
	}
	
	void UpdateBearing(float needleAngle)
	{
		float bearing = needleAngle;
		if (bearing > 0) bearing -= 360;
		// should we use Round here?
		bearing = Math.AbsFloat(bearing);
		if (bearing == 360) bearing = 0;
		
		TextWidget.Cast(m_wRoot.FindWidget("compassFrame.bearing")).SetText(bearing.ToString(3, 0));
		TextWidget.Cast(m_wRoot.FindWidget("compassFrame.cardinal"));
	}

	void SetCompassEntity(notnull IEntity compass)
	{
		m_CompassEntity = compass;
	}
	
	protected void OnUserSettingChanged()
	{
		m_Workspace = GetGame().GetWorkspace();
	}
	
}
