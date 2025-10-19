//------------------------------------------------------------------------------------------------
class ACE_ScreenGadgetComponentClass : SCR_GadgetComponentClass
{
}


//------------------------------------------------------------------------------------------------
class ACE_ScreenGadgetComponent : SCR_GadgetComponent
{
	[Attribute(desc: "Handlers for all screens")]
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
		
		if (!m_aScreenHandlers.IsEmpty())
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
	//! Called when some screen-relevant data on ACE_ScreenGadgetComponent got updated
	//! Add it to `onRplName` for relevant RplProps
	protected void OnRefreshScreen()
	{
		if (m_pCurrenScreenHandler && m_wRTTexture)
			m_pCurrenScreenHandler.OnRefreshScreen();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Forward button click to current screen handler
	void OnButtonClick(ACE_EGadgetButtonID buttonID)
	{
		Rpc(RpcDo_OnButtonClickServer, buttonID);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcDo_OnButtonClickServer(ACE_EGadgetButtonID buttonID)
	{
		if (m_pCurrenScreenHandler)
			m_pCurrenScreenHandler.OnButtonClickServer(buttonID);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Change screen
	void ChangeScreen(ACE_EGadgetScreenID nextID)
	{
		m_eCurrentScreenID = nextID;
		OnScreenChanged();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Change screen and push the old one to stack
	void PushScreen(ACE_EGadgetScreenID nextID)
	{
		m_aScreenIdStack.Insert(m_eCurrentScreenID);
		ChangeScreen(nextID);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Remove screen from stack and change to it
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
	//! Change to bottommost screen and clear stack
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
		if (m_pCurrenScreenHandler)
		{
			if (Replication.IsServer())
				m_pCurrenScreenHandler.OnDeactivateServer();
			
			if (m_wRTTexture)
				m_pCurrenScreenHandler.OnClose(m_wRTTexture);
		}
		
		foreach (ACE_IGadgetScreenHandler handler : m_aScreenHandlers)
		{
			if (handler.m_eID == m_eCurrentScreenID)
			{
				m_pCurrenScreenHandler = handler;
				break;
			}
		}
		
		if (m_pCurrenScreenHandler)
		{
			if (Replication.IsServer())
				m_pCurrenScreenHandler.OnActivateServer();
			
			if (m_wRTTexture)
				m_pCurrenScreenHandler.OnOpen(m_wRTTexture);
		}
	}
}
