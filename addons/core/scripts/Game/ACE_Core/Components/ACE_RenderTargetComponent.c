//------------------------------------------------------------------------------------------------
class ACE_RenderTargetComponentClass : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Component for configuring render target handled by ACE_RenderTargetSystem
class ACE_RenderTargetComponent : ScriptComponent
{
	[Attribute(uiwidget: UIWidgets.ResourcePickerThumbnail, desc: "Layout for the render target", params: "layout")]
	protected ResourceName m_sLayoutName;
	
	[Attribute(defvalue: "15", desc: "Maximum distance for render target being active [m]")]
	protected float m_fRenderDistanceM;
	protected float m_fRenderDistanceSq;
	
	[RplProp(onRplName: "OnToggleActive"), Attribute(defvalue: "false", desc: "Whether renter target is turned on")]
	protected bool m_bIsActive;
	
	[Attribute(defvalue: "false", desc: "Whether it is only rendered for the owner")]
	protected bool m_bOwnerOnly;
	
	protected Widget m_wRoot;
	protected RTTextureWidget m_wRTTexture;
	protected InventoryItemComponent m_pItemComponent;
	protected ref ScriptInvokerBool m_pOnToggleActive;
	protected ref ScriptInvokerBase<ScriptInvokerWidgetBool> m_pOnToggleRender;
	
	//------------------------------------------------------------------------------------------------
	void ACE_RenderTargetComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		if (!GetGame().InPlayMode())
			return;
		
		m_fRenderDistanceSq = Math.Pow(m_fRenderDistanceM, 2);
		
		ACE_RenderTargetSystem system = ACE_RenderTargetSystem.GetInstance(GetOwner().GetWorld());
		if (system && m_bIsActive)
			system.Register(this);
	}
	
	//------------------------------------------------------------------------------------------------
	void RequestToggleActive(bool active)
	{
		Rpc(RpcDo_ToggleActiveServer, active);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcDo_ToggleActiveServer(bool active)
	{
		m_bIsActive = active;
		OnToggleActive();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsToggledOn()
	{
		return m_bIsActive;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnToggleActive()
	{
		if (m_bIsActive == IsRendered())
			return;
		
		ACE_RenderTargetSystem system = ACE_RenderTargetSystem.GetInstance(GetOwner().GetWorld());
		if (!system)
			return;
		
		if (m_bIsActive)
			system.Register(this);
		else
			system.Unregister(this);
		
		if (m_pOnToggleActive)
			m_pOnToggleActive.Invoke(m_bIsActive);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Whether RT is active on local client
	bool IsRendered()
	{
		return m_wRoot;
	}
	
	//------------------------------------------------------------------------------------------------
	RTTextureWidget GetRTTexture()
	{
		return m_wRTTexture;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetRenderDistanceSq()
	{
		return m_fRenderDistanceSq;
	}
	
	//------------------------------------------------------------------------------------------------
	[Friend(ACE_RenderTargetSystem)]
	protected void ToggleRender(bool active)
	{
		if (active)
		{
			if (m_wRoot)
				return;
			
			m_wRoot = GetGame().GetWorkspace().CreateWidgets(m_sLayoutName);
			m_wRTTexture = RTTextureWidget.Cast(m_wRoot.FindAnyWidget("RTTexture0"));
			m_wRTTexture.SetRenderTarget(GetOwner());
		}
		
		if (m_pOnToggleRender)
			m_pOnToggleRender.Invoke(m_wRTTexture, active);
		
		if (!active)
		{
			if (m_wRTTexture && !GetOwner().IsDeleted())
				m_wRTTexture.RemoveRenderTarget(GetOwner());
			
			if (m_wRoot)
				m_wRoot.RemoveFromHierarchy();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Invoker for when the screen is turned on/off
	ScriptInvokerBool GetOnToggleActive()
	{
		if (!m_pOnToggleActive)
			m_pOnToggleActive = new ScriptInvokerBool();
		
		return m_pOnToggleActive;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Invoker for when the screen rendering starts/stops
	ScriptInvokerBase<ScriptInvokerWidgetBool> GetOnToggleRender()
	{
		if (!m_pOnToggleRender)
			m_pOnToggleRender = new ScriptInvokerBase<ScriptInvokerWidgetBool>();
		
		return m_pOnToggleRender;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns whether the local machine is allowed to render
	bool CanRenderLocal()
	{
		if (System.IsConsoleApp())
			return false;
		
		if (!m_bOwnerOnly)
			return true;
		
		RplComponent rpl = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		if (!rpl || rpl.IsOwner())
			return true;
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	void ~ACE_RenderTargetComponent()
	{
		if (!GetGame().InPlayMode())
			return;
		
		ACE_RenderTargetSystem system = ACE_RenderTargetSystem.GetInstance(GetOwner().GetWorld());
		if (system && m_bIsActive)
			system.Unregister(this);
	}
}
