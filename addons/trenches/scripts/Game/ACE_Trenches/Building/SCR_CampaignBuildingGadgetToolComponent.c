//------------------------------------------------------------------------------------------------
//! Extend E-tool for placing trenches outside of building editor mode
//! Open placement radial menu with CharacterInspect action
//! Place with GadgetActivate action
modded class SCR_CampaignBuildingGadgetToolComponent : SCR_GadgetComponent
{
	[Attribute(defvalue: "{9D44ED260F5BBBE0}Configs/ACE/Trenches.conf", desc: "Placement mode configurations", params: "conf")]
	protected ResourceName m_sACE_Trenches_ConfigName;
	protected ref ACE_Trenches_Config m_ACE_Trenches_Config;
	
	[Attribute(defvalue: "2.5", desc: "Distance in meters from player for E-tool placement preview")]
	protected float m_sACE_Trenches_PlacementDistanceM;
	
	[Attribute(defvalue: "1", desc: "Cooldown in seconds until placing action can be used again")]
	protected float m_fACE_Trenches_ActionCooldownS;
	protected float m_fACE_Trenches_LastActionTime = 0;
	
	[Attribute()]
	protected ref SCR_RadialMenuController m_ACE_Trenches_RadialMenuController;
		
	protected CameraBase m_pACE_Trenches_CharacterCamera;
	protected IEntity m_pACE_Trenches_PreviewEntity;
	protected bool m_bACE_Trenches_InHand = false;
	protected bool m_bACE_Trenches_CanPlace;
	protected ref array<vector> m_aACE_Trenches_RelPositionsToCheck = {};
	protected int m_iACE_Trenches_TmpSelectedPrefabIdx = 0;
	protected int m_iACE_Trenches_CurrentPrefabIdx = 0;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
		
		m_ACE_Trenches_Config = SCR_ConfigHelperT<ACE_Trenches_Config>.GetConfigObject(m_sACE_Trenches_ConfigName);
		
		m_ACE_Trenches_RadialMenuController.GetOnTakeControl().Insert(ACE_Trenches_OnTakeRadialMenuControl);
		m_ACE_Trenches_RadialMenuController.GetOnControllerChanged().Insert(ACE_Trenches_OnLostRadialMenuControl);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Building tool taken to hand
	override void ToolToHand()
	{
		super.ToolToHand();
		
		// ToolToHand gets triggered twice when changing between crouch and prone stance. We therefore
		// skip the execution if it was already called
		if (m_bACE_Trenches_InHand)
			return;
		
		m_bACE_Trenches_InHand = true;
		GetGame().GetInputManager().AddActionListener("CharacterInspect", EActionTrigger.DOWN, ToogleActiveAction);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Building tool out of hands -> end building mode.
	override void ToolToInventory()
	{
		super.ToolToInventory();
		
		if (!m_bACE_Trenches_InHand)
			return;
		
		m_bACE_Trenches_InHand = false;
		ToggleActive(false, SCR_EUseContext.FROM_INVENTORY);
		GetGame().GetInputManager().RemoveActionListener("CharacterInspect", EActionTrigger.DOWN, ToogleActiveAction);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Send request for placing trench 
	void ACE_Trenches_RequestPlace(int prefabIdx)
	{
		PlayerController playerController = GetGame().GetPlayerController();
		if (!playerController)
			return;
			
		SCR_CampaignBuildingNetworkComponent buildingNetworkComponent = SCR_CampaignBuildingNetworkComponent.Cast(playerController.FindComponent(SCR_CampaignBuildingNetworkComponent));
		if (!buildingNetworkComponent)
			return;
			
		vector transform[4];
		ACE_Trenches_GetPlacementTransform(transform);
		buildingNetworkComponent.ACE_Trenches_RequestPlace(m_ACE_Trenches_Config.m_aPlaceablePrefabNames[prefabIdx], transform);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Creates preview entity
	protected void ACE_Trenches_StartPlacementPreview(int prefabIdx)
	{
		// End preview if index is negative
		if (prefabIdx < 0)
		{
			ACE_Trenches_EndPlacementPreview();
			return;
		}
		
		// Delete old preview
		if (m_pACE_Trenches_PreviewEntity)
			SCR_EntityHelper.DeleteEntityAndChildren(m_pACE_Trenches_PreviewEntity);
		
		Resource res = Resource.Load(m_ACE_Trenches_Config.m_aPlaceablePrefabNames[prefabIdx]);
		if (!res)
			return;
		
		m_pACE_Trenches_CharacterCamera = GetGame().GetCameraManager().CurrentCamera();
		m_pACE_Trenches_PreviewEntity = SCR_PrefabPreviewEntity.SpawnPreviewFromPrefab(res, "SCR_PrefabPreviewEntity", null, null, m_ACE_Trenches_Config.m_sCanPlacePreviewMaterial);
		
		m_bACE_Trenches_CanPlace = true;
		// Compile center and bounding box edges for future placement checks
		m_aACE_Trenches_RelPositionsToCheck = {vector.Zero};
		array<IEntity> children = {};
		SCR_EntityHelper.GetHierarchyEntityList(m_pACE_Trenches_PreviewEntity, children);
		
		foreach (IEntity child : children)
		{
			vector mins, maxs;
			child.GetBounds(mins, maxs);
			
			if (mins == maxs)
				continue;
			
			m_aACE_Trenches_RelPositionsToCheck.Insert(mins);
			m_aACE_Trenches_RelPositionsToCheck.Insert(maxs);
			m_aACE_Trenches_RelPositionsToCheck.Insert(Vector(mins[0], 0, maxs[2]));
			m_aACE_Trenches_RelPositionsToCheck.Insert(Vector(maxs[0], 0, mins[2]));
		}
		
		SCR_UISoundEntity.SoundEvent("SOUND_E_PLACE_GSTART");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Deletes preview entity
	protected void ACE_Trenches_EndPlacementPreview()
	{
		SCR_EntityHelper.DeleteEntityAndChildren(m_pACE_Trenches_PreviewEntity);
		SCR_UISoundEntity.SoundEvent("SOUND_HUD_GADGET_CANCEL");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get transform for placement in front of the player
	protected void ACE_Trenches_GetPlacementTransform(out vector transform[4])
	{
		m_pACE_Trenches_CharacterCamera.GetWorldTransform(transform);
		vector vectorDir = transform[2];
		vectorDir[1] = 0;
		vectorDir.Normalize();
		Math3D.AnglesToMatrix(Vector(vectorDir.ToYaw(), 0, 0), transform);
		transform[3] = m_CharacterOwner.GetOrigin() + m_sACE_Trenches_PlacementDistanceM * vectorDir;
		SCR_TerrainHelper.SnapAndOrientToTerrain(transform);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update transform of preview entity
	override void Update(float timeSlice)
	{
		super.Update(timeSlice);
		
		if (!m_pACE_Trenches_PreviewEntity)
			return;
		
		vector transform[4];
		ACE_Trenches_GetPlacementTransform(transform);
		m_pACE_Trenches_PreviewEntity.SetWorldTransform(transform);
		m_pACE_Trenches_PreviewEntity.Update();
		ACE_Trenches_UpdateCanPlace();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Determines whether trench can be placed based on the terrain surface
	protected void ACE_Trenches_UpdateCanPlace()
	{
		foreach (vector relPos : m_aACE_Trenches_RelPositionsToCheck)
		{
			SurfaceProperties props = ACE_TerrainHelper.GetSurfaceProperties(m_pACE_Trenches_PreviewEntity.CoordToParent(relPos));
			if (!props)
				continue;
			
			ResourceName currentSurface = props.GetResourceName();
			
			foreach (ResourceName surface : m_ACE_Trenches_Config.m_aBlacklistedSurfaces)
			{
				if (currentSurface == surface)
				{
					if (!m_bACE_Trenches_CanPlace)
						return;
	
					m_bACE_Trenches_CanPlace = false;
					SCR_Global.SetMaterial(m_pACE_Trenches_PreviewEntity, m_ACE_Trenches_Config.m_sCannotPlacePreviewMaterial);
					return;
				}
			}
		}
		
		if (m_bACE_Trenches_CanPlace)
			return;
		
		m_bACE_Trenches_CanPlace = true;
		SCR_Global.SetMaterial(m_pACE_Trenches_PreviewEntity, m_ACE_Trenches_Config.m_sCanPlacePreviewMaterial);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Toggles building placement preview and radial menu
	override void ToggleActive(bool state, SCR_EUseContext context)
	{
		// Inspect and quickslot keybinds start placement and toggle radial menu
		if (context == SCR_EUseContext.FROM_ACTION || context == SCR_EUseContext.FROM_QUICKSLOT)
		{
			m_bActivated = true;
			
			if (!m_ACE_Trenches_RadialMenuController.IsMenuOpen())
				m_ACE_Trenches_RadialMenuController.OnInputOpen();
			else
				m_ACE_Trenches_RadialMenuController.CloseMenu();
			
			return;
		}
		
		// Canceling of placement is done via radial menu (= SCR_EUseContext.CUSTOM) or
		// putting the E-tool to away (= SCR_EUseContext.FROM_INVENTORY)
		if (context != SCR_EUseContext.CUSTOM)
		{
			m_ACE_Trenches_RadialMenuController.CloseMenu();
			
			if (m_pACE_Trenches_PreviewEntity)
				ACE_Trenches_EndPlacementPreview();
		}
		
		m_bActivated = false;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Callback for GadgetActivate keybind
	//! Place object if placement mode is active
	override void ActivateAction()
	{		
		if (!m_bActivated || !m_bACE_Trenches_CanPlace)
			return;
		
		if (m_ACE_Trenches_RadialMenuController.IsMenuOpen())
			return;
		
		float cooldown = m_fACE_Trenches_ActionCooldownS - System.GetTickCount(m_fACE_Trenches_LastActionTime) / 1000;
		if (m_fACE_Trenches_ActionCooldownS > 0 && cooldown > 0)
		{
			SCR_NotificationsComponent.SendLocal(ENotification.ACTION_ON_COOLDOWN, cooldown * 100);
			return;
		}
		
		m_fACE_Trenches_LastActionTime = System.GetTickCount();
		ACE_Trenches_RequestPlace(m_iACE_Trenches_CurrentPrefabIdx);
		SCR_UISoundEntity.SoundEvent("SOUND_E_ACE_TRENCH_BUILD");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Callback for CharacterInspect keybind
	void ToogleActiveAction()
	{		
		ToggleActive(!m_bActivated, SCR_EUseContext.FROM_ACTION);
	}
	
	//----------------------------------------------------------------------------------------
	//! Initialize radial menu
	protected void ACE_Trenches_OnTakeRadialMenuControl(SCR_RadialMenuController controller)
	{
		SCR_RadialMenu menu = controller.GetRadialMenu();
		if (!menu)
			return;
		
		controller.UpdateMenuData();
		
		foreach (int idx, ResourceName resName : m_ACE_Trenches_Config.m_aPlaceablePrefabNames)
		{
			Resource res = Resource.Load(resName);
			if (!res.IsValid())
				continue;
			
			SCR_EditableEntityUIInfo info = ACE_BaseContainerTools.GetEditableEntityUIInfo(res.GetResource());
			if (!info)
				continue;
				
			SCR_SelectionMenuEntry entry = new SCR_SelectionMenuEntry();
			entry.SetName(info.GetName());
			entry.SetIcon(info.GetImage());
			entry.SetId(idx.ToString());
			menu.AddEntry(entry);
		}
		
		menu.GetOnSelect().Insert(ACE_Trenches_OnRadialMenuEntrySelected);
		menu.GetOnPerform().Insert(ACE_Trenches_OnRadialMenuEntryPerformed);
		menu.ACE_GetOnBackInputClose().Insert(ACE_Trenches_OnRadialMenuBackInputClosed);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Detach radial menu handlers
	protected void ACE_Trenches_OnLostRadialMenuControl(SCR_RadialMenuController controller, bool hasControl)
	{
		SCR_RadialMenu menu = controller.GetRadialMenu();
		if (!menu || hasControl)
			return;
		
		menu.GetOnSelect().Remove(ACE_Trenches_OnRadialMenuEntrySelected);
		menu.GetOnPerform().Remove(ACE_Trenches_OnRadialMenuEntryPerformed);
		menu.ACE_GetOnBackInputClose().Remove(ACE_Trenches_OnRadialMenuBackInputClosed);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update preview when new entry gets selected
	protected void ACE_Trenches_OnRadialMenuEntrySelected(SCR_RadialMenu menu, SCR_SelectionMenuEntry entry)
	{
		if (!entry)
			return;
		
		m_iACE_Trenches_TmpSelectedPrefabIdx = entry.GetId().ToInt();
		ACE_Trenches_StartPlacementPreview(m_iACE_Trenches_TmpSelectedPrefabIdx);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update the current prefab when entry is performed
	protected void ACE_Trenches_OnRadialMenuEntryPerformed(SCR_RadialMenu menu, SCR_SelectionMenuEntry entry)
	{
		m_iACE_Trenches_CurrentPrefabIdx = m_iACE_Trenches_TmpSelectedPrefabIdx;
		
		//! Cancel placement when no prefab was selected
		if (m_iACE_Trenches_CurrentPrefabIdx < 0)
			ToggleActive(false, SCR_EUseContext.CUSTOM);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Executed when radial menu is closed via back input
	protected void ACE_Trenches_OnRadialMenuBackInputClosed(SCR_RadialMenu menu)
	{
		// Revert back to previewing old prefab if no entry was performed
		if (m_iACE_Trenches_CurrentPrefabIdx != m_iACE_Trenches_TmpSelectedPrefabIdx)
		{
			m_iACE_Trenches_TmpSelectedPrefabIdx = m_iACE_Trenches_CurrentPrefabIdx;
			ACE_Trenches_StartPlacementPreview(m_iACE_Trenches_CurrentPrefabIdx);
		}
		
		//! Cancel placement when no prefab was selected
		if (m_iACE_Trenches_CurrentPrefabIdx < 0)
			ToggleActive(false, SCR_EUseContext.CUSTOM);
	}
}
