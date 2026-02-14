//------------------------------------------------------------------------------------------------
class ACE_Weather_WindInfoDisplay : SCR_InfoDisplayExtended
{
	protected ChimeraCharacter m_CharacterEntity;
	protected TNodeId m_CharHeadBone;
	protected TimeAndWeatherManagerEntity m_WeatherManager;
	protected Widget m_wLayout;
	protected CanvasWidget m_wCanvas;
	protected vector m_vCanvasCenter;
	protected ref LineDrawCommand m_MainLine = new LineDrawCommand();
	protected ref LineDrawCommand m_InnerCircle = new LineDrawCommand();
	protected ref array<ref CanvasWidgetCommand> m_aDrawCommands = {m_InnerCircle, m_MainLine};
	protected ref array<float> m_aOuterCircleVertices;
	
	// Lengths with respect to a native widget size of 256x256
	protected const float MAIN_LINE_LENGTH = 2 * 48;
	protected const float LINE_WIDTH = 2 * 3;
	protected const float TEN_KNOTS_BARB_LENGTH = 2 * 20;
	protected const float FIVE_KNOTS_BARB_LENGTH = 2 * 10;
	protected const float INTER_FEATHER_DIST = 2 * 7;
	protected const float FEATHER_ANGLE_RAD = 0.3;
	protected const float INNER_CIRCLE_RADIUS = 2 * 5;
	protected const float OUTER_CIRCLE_RADIUS = 2 * 15;
	protected const int CRICLE_VERTEX_COUNT = 16;
	
	protected float m_fDPIScale;
	protected float m_fMainLineLengthScaled;
	protected float m_fLineWidthScaled;
	protected float m_fTenKnotsBarbLengthScaled;
	protected float m_fFiveKnotsBarbLengthScaled;
	protected float m_fInterFeatherDistScaled;
	protected float m_fInnerCircleRadiusScaled;
	protected float m_fOuterCircleRadiusScaled;
	
	//------------------------------------------------------------------------------------------------
	override void DisplayStartDraw(IEntity owner)
	{
		ChimeraWorld world = GetGame().GetWorld();
		m_WeatherManager = world.GetTimeAndWeatherManager();
		m_wLayout = m_wRoot.FindWidget("Layout");
		m_wCanvas = CanvasWidget.Cast(m_wLayout.FindWidget("Canvas"));
		m_InnerCircle.m_iColor = Color.WHITE;
		m_MainLine.m_iColor = Color.WHITE;
		GetGame().GetInputManager().AddActionListener("ACE_Weather_WindInfoToggle", EActionTrigger.DOWN, ToggleDisplay);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateDPIScale(float scale)
	{
		m_fDPIScale = scale;
		m_vCanvasCenter = m_fDPIScale * 0.5 * FrameSlot.GetSize(m_wLayout);
		m_fMainLineLengthScaled = m_fDPIScale * MAIN_LINE_LENGTH;
		m_fLineWidthScaled = m_fDPIScale * LINE_WIDTH;
		m_fTenKnotsBarbLengthScaled = m_fDPIScale * TEN_KNOTS_BARB_LENGTH;
		m_fFiveKnotsBarbLengthScaled = m_fDPIScale * FIVE_KNOTS_BARB_LENGTH;
		m_fInterFeatherDistScaled = m_fDPIScale * INTER_FEATHER_DIST;
		m_fInnerCircleRadiusScaled = m_fDPIScale * INNER_CIRCLE_RADIUS;
		m_fOuterCircleRadiusScaled = m_fDPIScale * OUTER_CIRCLE_RADIUS;
		m_InnerCircle.m_fWidth = m_fLineWidthScaled;
		m_MainLine.m_fWidth = m_fLineWidthScaled;
	}
	
	//------------------------------------------------------------------------------------------------
	override void DisplayStopDraw(IEntity owner)
	{
		GetGame().GetInputManager().RemoveActionListener("ACE_Weather_WindInfoToggle", EActionTrigger.DOWN, ToggleDisplay);
	}
	
	//------------------------------------------------------------------------------------------------
	override void DisplayControlledEntityChanged(IEntity from, IEntity to)
	{
		Show(false);
		
		m_CharacterEntity = ChimeraCharacter.Cast(to);
		if (!m_CharacterEntity)
			return;
		
		m_CharHeadBone = m_CharacterEntity.GetAnimation().GetBoneIndex("Head");
	}

	//------------------------------------------------------------------------------------------------
	override protected void DisplayUpdate(IEntity owner, float timeSlice)
	{
		if (!IsShown() || !m_CharacterEntity)
			return;
		
		float dpiScale = GetGame().GetWorkspace().DPIScale(1);
		if (m_fDPIScale != dpiScale)
		{
			UpdateDPIScale(dpiScale);
			UpdateCirclesVertices();
		}
		
		UpdateWindBarbs();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateWindBarbs()
	{
		m_aDrawCommands = {m_InnerCircle, m_MainLine};
		
		int fiveKnotsCount = Math.Round(ACE_PhysicalConstants.MPS2KNOTS * m_WeatherManager.GetWindSpeed() / 5.0);
		if (fiveKnotsCount == 0)
		{
			// Draw an outer circle if wind is too faint
			m_MainLine.m_Vertices = m_aOuterCircleVertices;
			m_wCanvas.SetDrawCommands(m_aDrawCommands);
			return;
		}
		
		// Get wind direction relative to head
		vector localHeadTransform[4];
		m_CharacterEntity.GetAnimation().GetBoneMatrix(m_CharHeadBone, localHeadTransform);
		float headDir = m_CharacterEntity.VectorToParent(localHeadTransform[0]).ToYaw();
		float windDir = m_WeatherManager.GetWindDirection();
		float windAngleRad = Math.DEG2RAD * (windDir - headDir);
		vector screenWindVec = Vector(-Math.Cos(windAngleRad), -Math.Sin(windAngleRad), 0);
		vector screenPerpVec = Vector(screenWindVec[1], -screenWindVec[0], 0);
		
		vector mainStart = m_vCanvasCenter - m_fInnerCircleRadiusScaled * screenWindVec;
		vector mainEnd = m_vCanvasCenter - m_fMainLineLengthScaled * screenWindVec;
		m_MainLine.m_Vertices = {mainStart[0], mainStart[1], mainEnd[0], mainEnd[1]};
		vector featherPos = mainEnd;
		
		m_aDrawCommands.Reserve(m_aDrawCommands.Count() + Math.Ceil(0.5 * fiveKnotsCount));
		
		// Single 5 knots barb is offset for clarification
		if (fiveKnotsCount == 1)
			featherPos += m_fInterFeatherDistScaled * screenWindVec;
		
		while (fiveKnotsCount > 0)
		{
			float featherLen = m_fTenKnotsBarbLengthScaled;
			
			if (fiveKnotsCount >= 10)
			{
				m_aDrawCommands.Insert(GetPennantDrawCommand(featherPos, featherLen, screenWindVec, screenPerpVec));
				fiveKnotsCount -= 10;
			}
			else
			{
				if (fiveKnotsCount == 1)
					featherLen = m_fFiveKnotsBarbLengthScaled;
				
				m_aDrawCommands.Insert(GetBarbDrawCommand(featherPos, featherLen, screenWindVec, screenPerpVec));
				fiveKnotsCount -= 2;
			}
			
			featherPos += m_fInterFeatherDistScaled * screenWindVec;
		}
		
		m_wCanvas.SetDrawCommands(m_aDrawCommands);
	}
	
	//------------------------------------------------------------------------------------------------
	protected CanvasWidgetCommand GetPennantDrawCommand(inout vector featherPos, float featherLen, vector screenWindVec, vector screenPerpVec)
	{
		PolygonDrawCommand pennant = new PolygonDrawCommand();
		pennant.m_iColor = Color.WHITE;
		featherPos += m_fLineWidthScaled * screenWindVec;
		vector start = featherPos + 0.5 * m_fLineWidthScaled * (screenWindVec - screenPerpVec);
		vector apex1 = featherPos + featherLen * (screenPerpVec * Math.Cos(FEATHER_ANGLE_RAD) - screenWindVec * Math.Sin(FEATHER_ANGLE_RAD));
		vector apex2 = apex1 - m_fLineWidthScaled * screenWindVec;
		vector end = start - featherLen * screenWindVec * Math.Sin(FEATHER_ANGLE_RAD) - m_fLineWidthScaled * screenWindVec;
		pennant.m_Vertices = {start[0], start[1], apex1[0], apex1[1], apex2[0], apex2[1], end[0], end[1]};
		return pennant;
	}
	
	//------------------------------------------------------------------------------------------------
	protected CanvasWidgetCommand GetBarbDrawCommand(inout vector featherPos, float featherLen, vector screenWindVec, vector screenPerpVec)
	{
		LineDrawCommand barb = new LineDrawCommand();
		barb.m_iColor = Color.WHITE;
		barb.m_fWidth = m_fLineWidthScaled;
		vector barbEnd = featherPos + featherLen * (screenPerpVec * Math.Cos(FEATHER_ANGLE_RAD) - screenWindVec * Math.Sin(FEATHER_ANGLE_RAD));
		barb.m_Vertices = {featherPos[0], featherPos[1], barbEnd[0], barbEnd[1]};
		return barb;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateCirclesVertices()
	{
		m_aOuterCircleVertices = {};
		m_aOuterCircleVertices.Reserve(2 * CRICLE_VERTEX_COUNT + 2);
		m_InnerCircle.m_Vertices = {};
		m_InnerCircle.m_Vertices.Reserve(2 * CRICLE_VERTEX_COUNT + 2);
		
		for (int i = 0; i <= CRICLE_VERTEX_COUNT; ++i)
		{
			float angle = i * Math.PI2 / CRICLE_VERTEX_COUNT;
			m_aOuterCircleVertices.Insert(m_vCanvasCenter[0] + m_fOuterCircleRadiusScaled * Math.Cos(angle));
			m_aOuterCircleVertices.Insert(m_vCanvasCenter[1] + m_fOuterCircleRadiusScaled * Math.Sin(angle));
			m_InnerCircle.m_Vertices.Insert(m_vCanvasCenter[0] + m_fInnerCircleRadiusScaled * Math.Cos(angle));
			m_InnerCircle.m_Vertices.Insert(m_vCanvasCenter[1] + m_fInnerCircleRadiusScaled * Math.Sin(angle));
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ToggleDisplay()
	{
		Show(!IsShown());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminate screen when chracter transitions from uncon to death
	override protected void DisplayConsciousnessChanged(bool conscious, bool init = false)
	{
		if (init || !conscious)
			return;
		
		if (m_CharacterController.GetLifeState() == ECharacterLifeState.DEAD)
			Show(false);
	}
}
