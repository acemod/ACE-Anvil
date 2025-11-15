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
	protected InventoryItemComponent m_pItemComponent;
	protected RplComponent m_pRplComponent;
	
	protected SCR_CharacterControllerComponent m_pOwnerCharController;
	protected ACE_InspectGadgetMenu m_pInspectionMenu;
	protected bool m_bIsInspecting = false;
	
	[Attribute(uiwidget: UIWidgets.ColorPicker, desc: "Emissive color of backlight material.")]
	protected int m_iEnabledBacklightColor;
	
	[RplProp(onRplName: "OnToggleBacklight")]
	protected bool m_bIsBacklightActive = false;
	
	// Color for backlight material. Applied via material via Refs:
	// Refs {
	//     "Emissive" "ACE_ScreenGadgetComponent.m_iBacklightColor"
	// }
	protected int m_iBacklightColor;
	
	protected static const int ENTER_RAISED_MODE_DELAY_MS = 350;
	
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
		m_pItemComponent = InventoryItemComponent.Cast(owner.FindComponent(InventoryItemComponent));
		m_pRplComponent = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		
		if (m_pRenderTargetComponent.IsToggledOn())
			OnToggleScreenActive(true);
		
		m_pRenderTargetComponent.GetOnToggleActive().Insert(OnToggleScreenActive);
		
		if (m_pRenderTargetComponent.IsRendered())
			OnToggleRenderScreen(m_pRenderTargetComponent.GetRTTexture(), true);
		
		m_pRenderTargetComponent.GetOnToggleRender().Insert(OnToggleRenderScreen);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Give ownership to player
	override void ModeSwitch(EGadgetMode mode, IEntity charOwner)
	{
		super.ModeSwitch(mode, charOwner);
		
		// Gadget has to be tracable for physical buttons to work
		m_pItemComponent.SetTraceable(mode != EGadgetMode.IN_SLOT);
		
		if (charOwner)
			m_pOwnerCharController = SCR_CharacterControllerComponent.Cast(charOwner.FindComponent(SCR_CharacterControllerComponent));
		else
			m_pOwnerCharController = null;
		
		if (m_pRplComponent.IsProxy())
			return;
		
		m_pRenderTargetComponent.RequestToggleActive(EGadgetMode.IN_HAND == mode);
		
		RplIdentity identity = RplIdentity.Local();
		int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(charOwner);
		if (playerID > 0)
		{
			PlayerController playerController = GetGame().GetPlayerManager().GetPlayerController(playerID);
			if (playerController)
				identity = playerController.GetRplIdentity();
		}
		
		m_pRplComponent.Give(identity);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnToggleScreenActive(bool active)
	{
		OnToggleBacklight();
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
		
		if (!m_pRplComponent.IsOwner())
			return;
		
		bool isInspecting = m_pOwnerCharController.GetInspect();
		if (isInspecting == m_bIsInspecting)
			return;
		
		m_bIsInspecting = isInspecting;
		GetGame().GetCallqueue().Remove(InitInspectionMenu);
		
		if (m_bIsInspecting)
		{
			if (m_pOwnerCharController.IsGadgetRaisedModeWanted())
			{
				InitInspectionMenu();
			}
			else
			{
				m_pOwnerCharController.SetGadgetRaisedModeWanted(true);
				GetGame().GetCallqueue().CallLater(InitInspectionMenu, ENTER_RAISED_MODE_DELAY_MS);
			}
		}
		else
		{
			if (m_pInspectionMenu)
				GetGame().GetMenuManager().CloseMenu(m_pInspectionMenu);
			
			m_pInspectionMenu = null;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void InitInspectionMenu()
	{
		if (!m_bIsInspecting)
			return;
		
		m_pInspectionMenu = ACE_InspectGadgetMenu.Cast(GetGame().GetMenuManager().OpenDialog(ChimeraMenuPreset.ACE_InspectGadgetMenu, DialogPriority.INFORMATIVE, 0, true));
		
		ACE_PhysicalButtonsComponent buttonComponent = ACE_PhysicalButtonsComponent.Cast(GetOwner().FindComponent(ACE_PhysicalButtonsComponent));
		if (m_pInspectionMenu && buttonComponent)
			m_pInspectionMenu.GetPhysicalButtonsUIComponent().SetPhysicalButtonsComponent(buttonComponent);
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
	
	//------------------------------------------------------------------------------------------------
	void RequestToggleBacklight(bool active)
	{
		Rpc(RpcDo_ToggleBacklightServer, active);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcDo_ToggleBacklightServer(bool active)
	{
		m_bIsBacklightActive = active;
		OnToggleBacklight();
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsBacklightActive()
	{
		return m_bIsBacklightActive;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnToggleBacklight()
	{
		if (m_pRenderTargetComponent.IsToggledOn() && m_bIsBacklightActive)
			m_iBacklightColor = m_iEnabledBacklightColor;
		else
			m_iBacklightColor = Color.BLACK;
	}
}
