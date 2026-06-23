//------------------------------------------------------------------------------------------------
class ACE_MapDeclinationUIComponent : ScriptedWidgetComponent
{
	[Attribute("-16777216", UIWidgets.ColorPicker)]
	protected int m_iColor;
	
	protected SizeLayoutWidget m_wLayout;
	protected CanvasWidget m_wCanvas;
	protected vector m_vCenter;
	protected ImageWidget m_wStarIcon;
	protected TextWidget m_wMagneticNorthLabel;
	protected TextWidget m_wDeclinationText;
	protected float m_fDPIScale;
	protected vector m_vUnscaledMagneticVec;
	protected vector m_vUnscaledPerpVec;
	protected ref array<ref CanvasWidgetCommand> m_aDrawCommands;
	
	protected const float TN_LINE_LENGTH = 2 * 104;
	protected const float TN_LINE_WIDTH = 2;
	protected const float MN_LINE_LENGTH = TN_LINE_LENGTH - 30;
	protected const float MN_LINE_WIDTH = 4;
	protected const float MN_LABEL_OFFSET = 15;
	protected const float TEXT_OFFSET = 6;
	protected const float ARROW_HEAD_LENGTH = 18;
	protected const float ARROW_HEAD_WIDTH = 6;
	
	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		m_wLayout = SizeLayoutWidget.Cast(w.FindAnyWidget("Layout"));
		m_wCanvas = CanvasWidget.Cast(m_wLayout.FindAnyWidget("Canvas"));
		m_wStarIcon = ImageWidget.Cast(m_wLayout.FindAnyWidget("Star"));
		m_wMagneticNorthLabel = TextWidget.Cast(m_wLayout.FindAnyWidget("MN"));
		m_wDeclinationText = TextWidget.Cast(m_wLayout.FindAnyWidget("Declination"));
		
		ChimeraWorld world = GetGame().GetWorld();
		if (!world)
			return;
		
		TimeAndWeatherManagerEntity manager = world.GetTimeAndWeatherManager();
		if (!manager)
			return;
		
		float declination = manager.ACE_GetMagneticDeclination();
		float rotation = Math.DEG2RAD * declination;
		m_vUnscaledMagneticVec = Vector(Math.Sin(rotation), -Math.Cos(rotation), 0);
		m_vUnscaledPerpVec = (float)declination.Sign() * Vector(-m_vUnscaledMagneticVec[1], m_vUnscaledMagneticVec[0], 0);
		
		m_wMagneticNorthLabel.SetRotation(declination);
		m_wMagneticNorthLabel.SetColorInt(m_iColor);
		FrameSlot.SetAlignment(m_wMagneticNorthLabel, declination < 0, 0.5);
		
		m_wDeclinationText.SetTextFormat("%1°", declination.ToString(lenDec: 1));
		m_wDeclinationText.SetColorInt(m_iColor);
		FrameSlot.SetAlignment(m_wDeclinationText, declination < 0, 0.5);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnUpdate(Widget w)
	{
		float dpiScale = GetGame().GetWorkspace().DPIScale(1);
		if (dpiScale == m_fDPIScale)
			return true;
		
		m_fDPIScale = dpiScale;
		m_vCenter = m_fDPIScale * Vector(0.5 * m_wLayout.GetWidthOverride(), m_wLayout.GetHeightOverride(), 0);
		vector magneticVec = m_fDPIScale * m_vUnscaledMagneticVec;
		vector perpVec = m_fDPIScale * m_vUnscaledPerpVec;
		
		vector endPos = m_vCenter - m_fDPIScale * TN_LINE_LENGTH * vector.Up;
		LineDrawCommand trueNorthLine = CreateLines({m_vCenter, endPos}, TN_LINE_WIDTH);
		SetWidgetPos(m_wStarIcon, endPos);
		
		endPos = m_vCenter + MN_LINE_LENGTH * magneticVec;
		vector arrowPos = m_vCenter + (MN_LINE_LENGTH - ARROW_HEAD_LENGTH) * magneticVec + ARROW_HEAD_WIDTH * perpVec;
		LineDrawCommand magneticNorthLine = CreateLines({m_vCenter, endPos, arrowPos}, MN_LINE_WIDTH);		
		m_aDrawCommands = {trueNorthLine, magneticNorthLine};
		m_wCanvas.SetDrawCommands(m_aDrawCommands);
		
		SetWidgetPos(m_wMagneticNorthLabel, m_vCenter + (MN_LINE_LENGTH + MN_LABEL_OFFSET) * magneticVec + TEXT_OFFSET * perpVec);
		SetWidgetPos(m_wDeclinationText, m_vCenter + (MN_LINE_LENGTH / 2) * magneticVec +  TEXT_OFFSET * perpVec);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void SetWidgetPos(Widget w, vector pos)
	{
		pos /= m_fDPIScale;
		FrameSlot.SetPos(w, pos[0], pos[1]);
	}
	
	//------------------------------------------------------------------------------------------------
	protected LineDrawCommand CreateLines(array<vector> points, float lineWidth)
	{
		LineDrawCommand command = new LineDrawCommand();
		command.m_iColor = m_iColor;
		command.m_fWidth = m_fDPIScale * lineWidth;
		command.m_Vertices = {};
		
		foreach (vector point : points)
		{
			command.m_Vertices.Insert(point[0]);
			command.m_Vertices.Insert(point[1]);
		}
		
		return command;
	}
}
