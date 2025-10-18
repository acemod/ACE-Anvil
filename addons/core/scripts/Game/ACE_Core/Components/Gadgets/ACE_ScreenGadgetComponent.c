//------------------------------------------------------------------------------------------------
class ACE_ScreenGadgetComponentClass : SCR_GadgetComponentClass
{
}


//------------------------------------------------------------------------------------------------
class ACE_ScreenGadgetComponent : SCR_GadgetComponent
{
	[Attribute()]
	protected ref array<ref ACE_IGadgetScreenHandler> m_aScreenHandlers;
	
	[RplProp(onRplName: "OnScreenChanged")]
	protected ACE_EGadgetScreenID m_eCurrentScreenID;
	protected ACE_IGadgetScreenHandler m_pCurrenScreenHandler;
	
	protected ref array<ACE_EGadgetScreenID> m_aScreenIdStack = {};
	
	protected Widget m_wRTTexture;
	protected ACE_RenderTargetComponent m_pRenderTargetComponent;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
		
		foreach (ACE_IGadgetScreenHandler handler : m_aScreenHandlers)
		{
			handler.Init(this);
		}
		
		ChangeScreen(m_aScreenHandlers[0].m_eID);
		m_pRenderTargetComponent = ACE_RenderTargetComponent.Cast(owner.FindComponent(ACE_RenderTargetComponent));
		
		if (m_pRenderTargetComponent.IsRendered())
			OnToggleRenderScreen(m_pRenderTargetComponent.GetRTTexture(), true);
		
		m_pRenderTargetComponent.GetOnToggleRender().Insert(OnToggleRenderScreen);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Give ownership to player
	override void ModeSwitch(EGadgetMode mode, IEntity charOwner)
	{
		super.ModeSwitch(mode, charOwner);
		
		RplComponent rpl = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		if (!rpl || rpl.IsProxy())
			return;
		
		m_pRenderTargetComponent.ToggleActive(EGadgetMode.IN_HAND == mode);
		
		if (mode != EGadgetMode.ON_GROUND && mode != EGadgetMode.IN_STORAGE)
			return;
		
		RplIdentity identity = RplIdentity.Local();
		int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(charOwner);
		if (playerID > 0)
		{
			PlayerController playerController = GetGame().GetPlayerManager().GetPlayerController(playerID);
			if (playerController)
				identity = playerController.GetRplIdentity();
		}
		
		rpl.Give(identity);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnToggleRenderScreen(Widget rtTexture, bool active)
	{
		if (active)
		{
			m_wRTTexture = rtTexture;
			
			if (m_pCurrenScreenHandler)
				m_pCurrenScreenHandler.OnOpen(m_wRTTexture);
			
			ActivateGadgetUpdate();
		}
		else
		{
			DeactivateGadgetUpdate();
			
			if (m_pCurrenScreenHandler)
				m_pCurrenScreenHandler.OnClose(m_wRTTexture);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void Update(float timeSlice)
	{
		if (m_pCurrenScreenHandler)
			m_pCurrenScreenHandler.OnUpdate(timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnButtonClick(ACE_EGadgetButtonID buttonID)
	{
		if (m_pCurrenScreenHandler)
			m_pCurrenScreenHandler.OnButtonClick(buttonID);
	}
	
	//------------------------------------------------------------------------------------------------
	void ChangeScreen(ACE_EGadgetScreenID nextID)
	{
		m_eCurrentScreenID = nextID;
		OnScreenChanged();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void PushScreen(ACE_EGadgetScreenID nextID)
	{
		m_aScreenIdStack.Insert(m_eCurrentScreenID);
		ChangeScreen(nextID);
	}
	
	//------------------------------------------------------------------------------------------------
	bool PopScreen()
	{
		int i = m_aScreenIdStack.Count() - 1;
		if (i < 0)
			return false;
		
		ACE_EGadgetScreenID nextID = m_aScreenIdStack[i];
		m_aScreenIdStack.Remove(i);
		ChangeScreen(nextID);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool PopToRootScreen()
	{
		if (m_aScreenIdStack.IsEmpty())
			return false;
		
		ACE_EGadgetScreenID nextID = m_aScreenIdStack[0];
		m_aScreenIdStack.Clear();
		ChangeScreen(nextID);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnScreenChanged()
	{
		if (m_pCurrenScreenHandler && m_wRTTexture)
			m_pCurrenScreenHandler.OnClose(m_wRTTexture);
		
		foreach (ACE_IGadgetScreenHandler handler : m_aScreenHandlers)
		{
			if (handler.m_eID == m_eCurrentScreenID)
			{
				m_pCurrenScreenHandler = handler;
				break;
			}
		}
		
		if (m_pCurrenScreenHandler && m_wRTTexture)
			m_pCurrenScreenHandler.OnOpen(m_wRTTexture);
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_IGadgetScreenHandler GetCurrentScreenHandler()
	{
		return m_pCurrenScreenHandler;
	}
}
