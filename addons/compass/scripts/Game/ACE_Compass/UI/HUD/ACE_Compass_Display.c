//------------------------------------------------------------------------------------------------
class ACE_Compass_Display : SCR_InfoDisplayExtended
{
	const vector SCREEN_POS_OFFSET = "-50 0 0";
	
	protected IEntity m_CompassEntity;
	protected BaseWorld m_World;
	protected WorkspaceWidget m_Workspace;
	protected TextWidget m_wBearing;
	protected TextWidget m_wCardinal;
	
	//------------------------------------------------------------------------------------------------
	override void DisplayInit(IEntity owner)
	{
		super.DisplayInit(owner);

		m_World = GetGame().GetWorld();
		
		GetGame().OnUserSettingsChangedInvoker().Insert(OnUserSettingChanged);
		OnUserSettingChanged();
	}
	
	//------------------------------------------------------------------------------------------------
	override void DisplayStartDraw(IEntity owner)
	{
		super.DisplayStartDraw(owner);
		m_wBearing = TextWidget.Cast(m_wRoot.FindWidget("compassFrame.bearing"));
		m_wCardinal = TextWidget.Cast(m_wRoot.FindWidget("compassFrame.cardinal"));
	}

	//------------------------------------------------------------------------------------------------
	override protected void DisplayUpdate(IEntity owner, float timeSlice)
	{
		super.DisplayUpdate(owner, timeSlice);
		
		if (!m_bShown || !m_CompassEntity)
			return;

		vector worldPos = m_CompassEntity.GetOrigin();
		vector screenPos = m_Workspace.ProjWorldToScreen(worldPos, m_World);
		screenPos += SCREEN_POS_OFFSET;
				
		FrameSlot.SetPos(m_wRoot.FindWidget("compassFrame"), screenPos[0], screenPos[1]);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnShownFinished(Widget w, float targetOpacity, WidgetAnimationOpacity anim = null)
	{
		// fix the bug where HUD-reappears if fade-in finishes after hud was hidden (quick hud toggles)
		if (!m_bShown && targetOpacity > 0)
			Show(false);
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateBearing(float needleAngle)
	{
		float bearing = needleAngle;
		Print(needleAngle);
		if (bearing > 0)
			bearing -= 360;

		bearing = Math.Round(Math.AbsFloat(bearing));
		if (bearing == 360)
			bearing = 0;
		
		m_wBearing.SetText(bearing.ToString(3, 0));
		m_wCardinal.SetText(ACE_CompassTools.GetCardinalFromBearing(bearing));
	}
	
	//------------------------------------------------------------------------------------------------
	void SetCompassEntity(notnull IEntity compass)
	{
		m_CompassEntity = compass;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnUserSettingChanged()
	{
		m_Workspace = GetGame().GetWorkspace();
	}
	
}
