//------------------------------------------------------------------------------------------------
//! Extends editor ping display for finger pointing pings
modded class SCR_EditorPingInfoDisplay : SCR_InfoDisplay
{
	protected ACE_Finger_EditorComponent m_pACE_Finger_EditorComponent;
	
	//------------------------------------------------------------------------------------------------
	//! Callback for tactical ping keybinds
	//! Sends tracer from center of screen and sends a ping for the first intersection
	protected void ACE_Finger_SendPingAction(float value, EActionTrigger reason)
	{
		if (SCR_EditorManagerEntity.IsOpenedInstance())
			return;
		
		WorkspaceWidget workspace = GetGame().GetWorkspace();
		if (!workspace)
			return;
			
		int screenW = workspace.GetWidth();
		int screenH = workspace.GetHeight();
			
		BaseWorld world = GetGame().GetWorld();
		vector outDir;
		vector startPos = workspace.ProjScreenToWorld(workspace.DPIUnscale(screenW / 2), workspace.DPIUnscale(screenH / 2), outDir, world);
		outDir *= m_pACE_Finger_EditorComponent.GetMaxPointingDistance();
	
		TraceParam trace = new TraceParam();
		trace.Start = startPos;
		trace.End = startPos + outDir;
		trace.Flags = TraceFlags.WORLD | TraceFlags.OCEAN | TraceFlags.ENTS;
		trace.LayerMask = TRACE_LAYER_CAMERA;
		trace.Exclude = SCR_PlayerController.GetLocalMainEntity();
		float traceDis = world.TraceMove(trace, null);
		SCR_EditableEntityComponent	target = SCR_EditableEntityComponent.GetEditableEntity(trace.TraceEnt);
			
		if (!target && traceDis == 1)
			return; //<- No intersection
		
		m_pACE_Finger_EditorComponent.SendPing(startPos + outDir * traceDis, target);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Attach handlers when display is opened
	override event void OnStartDraw(IEntity owner)
	{
		super.OnStartDraw(owner);
		
		m_pACE_Finger_EditorComponent = ACE_Finger_EditorComponent.Cast(ACE_Finger_EditorComponent.GetInstance(ACE_Finger_EditorComponent, true));
		if (!m_pACE_Finger_EditorComponent) return;
		
		m_pACE_Finger_EditorComponent.GetOnPingEntityRegister().Insert(OnPingEntityRegister);
		m_pACE_Finger_EditorComponent.GetOnPingEntityUnregister().Insert(OnPingEntityUnregister);
		
		InputManager inputManager = GetGame().GetInputManager();
		inputManager.AddActionListener("TacticalPing", EActionTrigger.DOWN, ACE_Finger_SendPingAction);
		inputManager.AddActionListener("TacticalPingHold", EActionTrigger.DOWN, ACE_Finger_SendPingAction);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Detatch handlers when display is closed
	override event void OnStopDraw(IEntity owner)
	{
		super.OnStopDraw(owner);
		
		if (!m_pACE_Finger_EditorComponent) return;
		
		m_pACE_Finger_EditorComponent.GetOnPingEntityRegister().Remove(OnPingEntityRegister);
		m_pACE_Finger_EditorComponent.GetOnPingEntityUnregister().Remove(OnPingEntityUnregister);
		
		InputManager inputManager = GetGame().GetInputManager();
		inputManager.RemoveActionListener("TacticalPing", EActionTrigger.DOWN, ACE_Finger_SendPingAction);
		inputManager.RemoveActionListener("TacticalPingHold", EActionTrigger.DOWN, ACE_Finger_SendPingAction);
	}
}
