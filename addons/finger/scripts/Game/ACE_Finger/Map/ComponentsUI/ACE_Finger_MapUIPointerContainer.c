//------------------------------------------------------------------------------------------------
class ACE_Finger_MapUIPointerContainer : SCR_MapUIBaseComponent
{
	[Attribute("UIIconsContainer")];
	protected string m_sIconsContainer;
	protected Widget m_wIconsContainer;
	
	[Attribute("{DD15734EB89D74E2}UI/layouts/Map/MapMarkerBase.layout", params: "layout")]
	protected ResourceName m_sPointerElementName;
	
	protected ref array<ACE_Finger_MapPointer> m_aPointers = {};
	protected ref array<Widget> m_aWidgets = {};
	
	[Attribute(defvalue: "0.1", desc: "Pointer update timer [s]")]
	protected float m_fPointerUpdateTimeout;
	protected float m_fPointerUpdateTimer = 0;
		
	//------------------------------------------------------------------------------------------------
	override protected void OnMapOpen(MapConfiguration config)
	{
		super.OnMapOpen(config);
		
		ACE_Finger_MapPointingSystem manager = ACE_Finger_MapPointingSystem.GetInstance();
		if (manager)
			manager.RegisterContainer(this);
		
		m_wIconsContainer = m_RootWidget.FindAnyWidget(m_sIconsContainer);
		m_wIconsContainer.SetVisible(true);
		
		GetGame().GetInputManager().AddActionListener("ACE_Finger_MapPointing", EActionTrigger.DOWN, PointerOnAction);
		GetGame().GetInputManager().AddActionListener("ACE_Finger_MapPointing", EActionTrigger.UP, PointerOffAction);
	}
	
	//------------------------------------------------------------------------------------------------
	override void Update(float timeSlice)
	{
		super.Update(timeSlice);
		
		ACE_Finger_MapPointer ptr = ACE_Finger_MapPointer.GetLocalInstance();
		if (ptr && ptr.IsToggleOn())
		{
			m_fPointerUpdateTimer += timeSlice;
			if (m_fPointerUpdateTimer >= m_fPointerUpdateTimeout)
			{
				m_fPointerUpdateTimer = 0;
				
				int x, y;
				WidgetManager.GetMousePos(x, y);
				float xWorld, zWorld;
				m_MapEntity.ScreenToWorld(x, y, xWorld, zWorld);
				ptr.SetPos(Vector(xWorld, 0, zWorld));
			}
		}
		
		foreach (int i, ACE_Finger_MapPointer otherPtr: m_aPointers)
		{
			vector pos = otherPtr.GetPos();
			float x, y;
			m_MapEntity.WorldToScreen(pos[0], pos[2], x, y, true);
			FrameSlot.SetPos(m_aWidgets[i], GetGame().GetWorkspace().DPIUnscale(x), GetGame().GetWorkspace().DPIUnscale(y));
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnMapClose(MapConfiguration config)
	{
		super.OnMapClose(config);
		
		ACE_Finger_MapPointingSystem manager = ACE_Finger_MapPointingSystem.GetInstance();
		if (manager)
			manager.UnregisterContainer(this);
		
		
		
		GetGame().GetInputManager().RemoveActionListener("ACE_Finger_MapPointing", EActionTrigger.DOWN, PointerOnAction);
		GetGame().GetInputManager().RemoveActionListener("ACE_Finger_MapPointing", EActionTrigger.UP, PointerOffAction);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void PointerOnAction(float value, EActionTrigger reason)
	{
		ACE_Finger_MapPointer ptr = ACE_Finger_MapPointer.GetLocalInstance();
		if (!ptr)
			return;
		
		int x, y;
		WidgetManager.GetMousePos(x, y);
		float xWorld, zWorld;
		m_MapEntity.ScreenToWorld(x, y, xWorld, zWorld);
		ptr.SetPos(Vector(xWorld, 0, zWorld));
		ptr.ToggleActive(true);
		
		SCR_MapCursorModule curstorModule = SCR_MapCursorModule.Cast(m_MapEntity.GetMapModule(SCR_MapCursorModule));
		if (curstorModule)
			curstorModule.ACE_Finger_HandlePointing(true);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void PointerOffAction(float value, EActionTrigger reason)
	{
		ACE_Finger_MapPointer ptr = ACE_Finger_MapPointer.GetLocalInstance();
		if (!ptr)
			return;
		
		ptr.ToggleActive(false);
		
		SCR_MapCursorModule curstorModule = SCR_MapCursorModule.Cast(m_MapEntity.GetMapModule(SCR_MapCursorModule));
		if (curstorModule)
			curstorModule.ACE_Finger_HandlePointing(false);
	}
	
	//------------------------------------------------------------------------------------------------
	void AddPointer(ACE_Finger_MapPointer ptr)
	{
		if (ptr == ACE_Finger_MapPointer.GetLocalInstance())
			return;
		
		WorkspaceWidget workspace = GetGame().GetWorkspace();
		if (!workspace)
			return;
		
		Widget w = workspace.CreateWidgets(m_sPointerElementName, m_wIconsContainer);
		FrameSlot.SetAlignment(w, 0.5, 0.2);
		vector pos = ptr.GetPos();
		float x, y;
		m_MapEntity.WorldToScreen(pos[0], pos[2], x, y, true);
		FrameSlot.SetPos(w, workspace.DPIUnscale(x), workspace.DPIUnscale(y));
		
		SCR_MapMarkerWidgetComponent handler = SCR_MapMarkerWidgetComponent.Cast(w.FindHandler(SCR_MapMarkerWidgetComponent));
		if (!handler)
			return;
		
		handler.SetImage("{4020BDDA0BA7D5CF}UI/Textures/Icons/icons_gamepad/icons_gamepad.imageset", "stick_hold");
		handler.ACE_ResizeImage(0.6, 0.6);
		handler.SetColor(Color.Orange);
		
		PlayerController ctrl = PlayerController.Cast(ptr.GetOwner());
		if (ctrl && ctrl != GetGame().GetPlayerController())
			handler.SetAuthor(GetGame().GetPlayerManager().GetPlayerName(ctrl.GetPlayerId()));
		
		m_aPointers.Insert(ptr);
		m_aWidgets.Insert(w);
	}
	
	//------------------------------------------------------------------------------------------------
	void RemovePointer(ACE_Finger_MapPointer ptr)
	{
		if (ptr == ACE_Finger_MapPointer.GetLocalInstance())
			return;
		
		int i = m_aPointers.Find(ptr);
		if (i < 0)
			return;
		
		m_aPointers.Remove(i);
		m_aWidgets[i].RemoveFromHierarchy();
		m_aWidgets.Remove(i);
	}
	
	//------------------------------------------------------------------------------------------------
	void ClearPointers()
	{
		foreach (ACE_Finger_MapPointer ptr : m_aPointers)
		{
			RemovePointer(ptr);
		}
	}
}
