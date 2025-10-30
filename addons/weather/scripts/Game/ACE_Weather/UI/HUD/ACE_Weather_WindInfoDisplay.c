//------------------------------------------------------------------------------------------------
class ACE_Weather_WindInfoDisplay : SCR_InfoDisplayExtended
{
	protected ChimeraCharacter m_pCharacterEntity;
	protected TNodeId m_pCharHeadBone;
	protected TimeAndWeatherManagerEntity m_pWeatherManager;
	protected Widget m_wLayout;
	protected CanvasWidget m_wCanvas;
	protected vector m_vCanvasCenter;
	protected ref LineDrawCommand m_pMainLine = new LineDrawCommand();
	protected ref LineDrawCommand m_pInnerCircle = new LineDrawCommand();
	protected ref array<ref CanvasWidgetCommand> m_aDrawCommands = {m_pInnerCircle, m_pMainLine};
	protected ref array<float> m_aOuterCircleVertices;
	
	protected static const float MAIN_LINE_LENGTH = 48;
	protected static const float LINE_WIDTH = 3;
	protected static const float TEN_KNOTS_BARB_LENGTH = 20;
	protected static const float FIVE_KNOTS_BARB_LENGTH = 10;
	protected static const float INTER_FEATHER_DIST = 7;
	protected static const float FEATHER_ANGLE_RAD = 0.3;
	protected static const float INNER_CIRCLE_RADIUS = 5;
	protected static const float OUTER_CIRCLE_RADIUS = 15;
	protected static const int CRICLE_VERTEX_COUNT = 16;
	
	//------------------------------------------------------------------------------------------------
	override void DisplayStartDraw(IEntity owner)
	{
		ChimeraWorld world = GetGame().GetWorld();
		m_pWeatherManager = world.GetTimeAndWeatherManager();
		m_wLayout = m_wRoot.FindWidget("Layout");
		m_wCanvas = CanvasWidget.Cast(m_wLayout.FindWidget("Canvas"));
		m_vCanvasCenter = 0.5 * FrameSlot.GetSize(m_wLayout);
		m_pInnerCircle.m_iColor = Color.WHITE;
		m_pInnerCircle.m_fWidth = LINE_WIDTH;
		InitCirclesVertices();
		m_pMainLine.m_iColor = Color.WHITE;
		m_pMainLine.m_fWidth = LINE_WIDTH;
		GetGame().GetInputManager().AddActionListener("ACE_Weather_WindInfoToggle", EActionTrigger.DOWN, ToggleDisplay);
	}
	
	//------------------------------------------------------------------------------------------------
	override void DisplayStopDraw(IEntity owner)
	{
		GetGame().GetInputManager().RemoveActionListener("ACE_Weather_WindInfoToggle", EActionTrigger.DOWN, ToggleDisplay);
	}
	
	//------------------------------------------------------------------------------------------------
	override void DisplayControlledEntityChanged(IEntity from, IEntity to)
	{
		m_pCharacterEntity = ChimeraCharacter.Cast(to);
		if (!m_pCharacterEntity)
			return;
		
		m_pCharHeadBone = m_pCharacterEntity.GetAnimation().GetBoneIndex("Head");
	}

	//------------------------------------------------------------------------------------------------
	override protected void DisplayUpdate(IEntity owner, float timeSlice)
	{
		if (!m_pCharacterEntity)
			return;
		
		UpdateWindBarbs();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateWindBarbs()
	{
		m_aDrawCommands = {m_pInnerCircle, m_pMainLine};
		
		int fiveKnotsCount = Math.Round(ACE_PhysicalConstants.MPS2KNOTS * m_pWeatherManager.GetWindSpeed() / 5.0);
		if (fiveKnotsCount == 0)
		{
			// Draw an outer circle if wind is too faint
			m_pMainLine.m_Vertices = m_aOuterCircleVertices;
			m_wCanvas.SetDrawCommands(m_aDrawCommands);
			return;
		}
		
		// Get wind direction relative to head
		vector localHeadTransform[4];
		m_pCharacterEntity.GetAnimation().GetBoneMatrix(m_pCharHeadBone, localHeadTransform);
		float headDir = m_pCharacterEntity.VectorToParent(localHeadTransform[0]).ToYaw();
		float windDir = m_pWeatherManager.GetWindDirection();
		float windAngleRad = Math.DEG2RAD * (windDir - headDir);
		vector screenWindVec = Vector(-Math.Cos(windAngleRad), -Math.Sin(windAngleRad), 0);
		vector screenPerpVec = Vector(screenWindVec[1], -screenWindVec[0], 0);
		
		vector mainStart = m_vCanvasCenter - INNER_CIRCLE_RADIUS * screenWindVec;
		vector mainEnd = m_vCanvasCenter - MAIN_LINE_LENGTH * screenWindVec;
		m_pMainLine.m_Vertices = {mainStart[0], mainStart[1], mainEnd[0], mainEnd[1]};
		vector featherPos = mainEnd;
		
		m_aDrawCommands.Reserve(m_aDrawCommands.Count() + Math.Ceil(0.5 * fiveKnotsCount));
		
		// Single 5 knots barb is offset for clarification
		if (fiveKnotsCount == 1)
			featherPos += INTER_FEATHER_DIST * screenWindVec;
		
		while (fiveKnotsCount > 0)
		{
			float featherLen = TEN_KNOTS_BARB_LENGTH;
			
			if (fiveKnotsCount >= 10)
			{
				m_aDrawCommands.Insert(GetPennantDrawCommand(featherPos, featherLen, screenWindVec, screenPerpVec));
				fiveKnotsCount -= 10;
			}
			else
			{
				if (fiveKnotsCount == 1)
					featherLen = FIVE_KNOTS_BARB_LENGTH;
				
				m_aDrawCommands.Insert(GetBarbDrawCommand(featherPos, featherLen, screenWindVec, screenPerpVec));
				fiveKnotsCount -= 2;
			}
			
			featherPos += INTER_FEATHER_DIST * screenWindVec;
		}
		
		m_wCanvas.SetDrawCommands(m_aDrawCommands);
	}
	
	//------------------------------------------------------------------------------------------------
	protected CanvasWidgetCommand GetPennantDrawCommand(inout vector featherPos, float featherLen, vector screenWindVec, vector screenPerpVec)
	{
		PolygonDrawCommand pennant = new PolygonDrawCommand();
		pennant.m_iColor = Color.WHITE;
		featherPos += LINE_WIDTH * screenWindVec;
		vector start = featherPos + 0.5 * LINE_WIDTH * (screenWindVec - screenPerpVec);
		vector apex1 = featherPos + featherLen * (screenPerpVec * Math.Cos(FEATHER_ANGLE_RAD) - screenWindVec * Math.Sin(FEATHER_ANGLE_RAD));
		vector apex2 = apex1 - LINE_WIDTH * screenWindVec;
		vector end = start - featherLen * screenWindVec * Math.Sin(FEATHER_ANGLE_RAD) - LINE_WIDTH * screenWindVec;
		pennant.m_Vertices = {start[0], start[1], apex1[0], apex1[1], apex2[0], apex2[1], end[0], end[1]};
		return pennant;
	}
	
	//------------------------------------------------------------------------------------------------
	protected CanvasWidgetCommand GetBarbDrawCommand(inout vector featherPos, float featherLen, vector screenWindVec, vector screenPerpVec)
	{
		LineDrawCommand barb = new LineDrawCommand();
		barb.m_iColor = Color.WHITE;
		barb.m_fWidth = LINE_WIDTH;
		vector barbEnd = featherPos + featherLen * (screenPerpVec * Math.Cos(FEATHER_ANGLE_RAD) - screenWindVec * Math.Sin(FEATHER_ANGLE_RAD));
		barb.m_Vertices = {featherPos[0], featherPos[1], barbEnd[0], barbEnd[1]};
		return barb;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void InitCirclesVertices()
	{
		m_aOuterCircleVertices = {};
		m_aOuterCircleVertices.Reserve(2 * CRICLE_VERTEX_COUNT + 2);
		m_pInnerCircle.m_Vertices = {};
		m_pInnerCircle.m_Vertices.Reserve(2 * CRICLE_VERTEX_COUNT + 2);
		
		for (int i = 0; i <= CRICLE_VERTEX_COUNT; ++i)
		{
			float angle = i * Math.PI2 / CRICLE_VERTEX_COUNT;
			m_aOuterCircleVertices.Insert(m_vCanvasCenter[0] + OUTER_CIRCLE_RADIUS * Math.Cos(angle));
			m_aOuterCircleVertices.Insert(m_vCanvasCenter[1] + OUTER_CIRCLE_RADIUS * Math.Sin(angle));
			m_pInnerCircle.m_Vertices.Insert(m_vCanvasCenter[0] + INNER_CIRCLE_RADIUS * Math.Cos(angle));
			m_pInnerCircle.m_Vertices.Insert(m_vCanvasCenter[1] + INNER_CIRCLE_RADIUS * Math.Sin(angle));
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ToggleDisplay()
	{
		Show(!IsShown());
	}
}
