//------------------------------------------------------------------------------------------------
//! Widget component for forwarding left mouse button clicks to physical buttons
class ACE_PhysicalButtonsUIComponent : ScriptedWidgetComponent
{
	[Attribute("1", desc: "Length of tracer for detecting physical buttons [m]")]
	protected float m_fTracerLength;
	
	[Attribute(defvalue: "{7063BDFDF0B3998C}UI/layouts/Gagets/ACE_GamepadPhysicalButton.layout", uiwidget: UIWidgets.ResourceNamePicker, params: "layout")]
	protected ResourceName m_sGamepadButtonLayout;
	
	protected WorkspaceWidget m_wWorkspace;
	protected Widget m_wWidget;
	protected ACE_PhysicalButtonsComponent m_pPhysicalButtonsComponent;
	protected string m_sActiveButtonColliderName;

	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		m_wWidget = w;
		m_wWorkspace = GetGame().GetWorkspace();
	}
	
	//------------------------------------------------------------------------------------------------
	void SetPhysicalButtonsComponent(notnull ACE_PhysicalButtonsComponent component)
	{
		m_pPhysicalButtonsComponent = component;
		
	#ifdef PLATFORM_CONSOLE
		IEntity owner = m_pPhysicalButtonsComponent.GetOwner();
		if (!owner)
			return;
		
		Animation ownerAnim = owner.GetAnimation();
		if (!ownerAnim)
			return;
		
		vector modelToWorldTransform[4];
		owner.GetWorldTransform(modelToWorldTransform);
		
		// Create helper button widgets on console
		Widget button;
		foreach (ACE_PhysicalButtonConfig config : m_pPhysicalButtonsComponent.GetAllButtonConfigs())
		{
			// Project position of button bone on screen
			vector modelTransform[4];
			ownerAnim.GetBoneMatrix(ownerAnim.GetBoneIndex(config.m_sBoneName), modelTransform);
			vector worldTransform[4];
			Math3D.MatrixMultiply4(modelToWorldTransform, modelTransform, worldTransform);
			vector screenPos = m_wWorkspace.ProjWorldToScreen(worldTransform[3], GetGame().GetWorld());
			
			button = m_wWorkspace.CreateWidgets(m_sGamepadButtonLayout, m_wWidget);
			FrameSlot.SetPos(button, screenPos[0], screenPos[1]);
			
			ACE_GamepadPhysicalButtonUIComponent handler = ACE_GamepadPhysicalButtonUIComponent.Cast(button.FindHandler(ACE_GamepadPhysicalButtonUIComponent));
			if (handler)
				handler.SetPhysicalButton(m_pPhysicalButtonsComponent, config);	
		}
		
		if (button)
			m_wWorkspace.SetFocusedWidget(button);
	#endif
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (button != SCR_EMouseButtons.LEFT || !m_pPhysicalButtonsComponent || !m_sActiveButtonColliderName.IsEmpty())
			return false;
		
		m_sActiveButtonColliderName = TraceButtonCollider(x, y);
		if (m_sActiveButtonColliderName.IsEmpty())
			return false;
		
		m_pPhysicalButtonsComponent.SetButtonState(m_sActiveButtonColliderName, 1);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnMouseButtonUp(Widget w, int x, int y, int button)
	{
		if (button != SCR_EMouseButtons.LEFT || !m_pPhysicalButtonsComponent || m_sActiveButtonColliderName.IsEmpty())
			return false;
		
		m_pPhysicalButtonsComponent.SetButtonState(m_sActiveButtonColliderName, 0);
		m_sActiveButtonColliderName = "";
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected string TraceButtonCollider(int x, int y)
	{
		vector cameraDir;
		vector cameraPos = m_wWorkspace.ProjScreenToWorldNative(x, y, cameraDir, GetGame().GetWorld());
		
		TraceParam params = new TraceParam();
		params.Flags = TraceFlags.ENTS;
		params.TargetLayers = EPhysicsLayerDefs.Interaction;
		params.Start = cameraPos;
		params.End = cameraPos + m_fTracerLength * cameraDir;
		GetGame().GetWorld().TraceMove(params, null);
		return params.ColliderName;
	}
}
