//------------------------------------------------------------------------------------------------
//! System for handling render targets on local client
class ACE_RenderTargetSystem : GameSystem
{
	[Attribute(defvalue: "1", desc: "Time between updates [s]")]
	protected float m_fUpdateTimeoutS;
	protected float m_fUpdateTimerS = 0;
	
	protected ref array<ACE_RenderTargetComponent> m_aComponents = {};
	
	//------------------------------------------------------------------------------------------------
	static ACE_RenderTargetSystem GetInstance()
	{
		ChimeraWorld world = GetGame().GetWorld();
		return ACE_RenderTargetSystem.Cast(world.FindSystem(ACE_RenderTargetSystem));
	}
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(true)
			.SetUnique(false)
			.SetLocation(ESystemLocation.Client)
			.AddPoint(ESystemPoint.Frame);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		super.OnInit();
		Enable(!m_aComponents.IsEmpty());
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnUpdate(ESystemPoint point)
	{
		super.OnUpdate(point);
		m_fUpdateTimerS += GetWorld().GetTimeSlice();
		
		if (m_fUpdateTimerS < m_fUpdateTimeoutS)
			return;
		
		m_fUpdateTimerS = 0;
		
		vector camPos;
		GetCameraPos(camPos);
		
		foreach (ACE_RenderTargetComponent component : m_aComponents)
		{
			OnUpdateComponent(component, camPos);	
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnUpdateComponent(ACE_RenderTargetComponent component, vector camPos)
	{
		bool shouldBeRendered = vector.DistanceSq(camPos, component.GetOwner().GetOrigin()) <= component.GetRenderDistanceSq();
		if (shouldBeRendered != component.IsRendered())
			component._ToggleRender(shouldBeRendered);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool GetCameraPos(out vector pos)
	{
		CameraBase camera = GetGame().GetCameraManager().CurrentCamera();
		if (!camera)
			return false;
		
		pos = camera.GetOrigin();
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	void Register(notnull ACE_RenderTargetComponent component)
	{
		m_aComponents.Insert(component);
		
		vector camPos;
		if (GetCameraPos(camPos))
			OnUpdateComponent(component, camPos);
		
		if (!IsEnabled())
			Enable(true);
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(ACE_RenderTargetComponent component)
	{
		m_aComponents.RemoveItem(component);
		
		if (component.IsRendered())
			component._ToggleRender(false);
		
		if (m_aComponents.IsEmpty())
			Enable(false);
	}
}
