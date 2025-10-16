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
	
	protected Widget m_wRoot;
	protected RTTextureWidget m_wRTTexture;
	protected InventoryItemComponent m_pItemComponent;
	
	//------------------------------------------------------------------------------------------------
	void ACE_RenderTargetComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		if (!GetGame().InPlayMode())
			return;
		
		m_fRenderDistanceSq = Math.Pow(m_fRenderDistanceM, 2);
		
		ACE_RenderTargetSystem system = ACE_RenderTargetSystem.GetInstance();
		if (system && m_bIsActive)
			system.Register(this);
	}
	
	//------------------------------------------------------------------------------------------------
	void ToggleActive(bool active)
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
		
		ACE_RenderTargetSystem system = ACE_RenderTargetSystem.GetInstance();
		if (!system)
			return;
		
		if (m_bIsActive)
			system.Register(this);
		else
			system.Unregister(this);
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
	//! Friend setter for ACE_RenderTargetSystem
	void _ToggleRender(bool active)
	{
		if (active)
		{
			if (m_wRoot)
				return;
			
			m_wRoot = GetGame().GetWorkspace().CreateWidgets(m_sLayoutName);
			m_wRTTexture = RTTextureWidget.Cast(m_wRoot.FindAnyWidget("RTTexture0"));
			m_wRTTexture.SetRenderTarget(GetOwner());
		}
		else
		{
			if (m_wRTTexture)
				m_wRTTexture.RemoveRenderTarget(GetOwner());
			
			if (m_wRoot)
				m_wRoot.RemoveFromHierarchy();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void ~ACE_RenderTargetComponent()
	{
		if (!GetGame().InPlayMode())
			return;
		
		ACE_RenderTargetSystem system = ACE_RenderTargetSystem.GetInstance();
		if (system && m_bIsActive)
			system.Unregister(this);
	}
}
