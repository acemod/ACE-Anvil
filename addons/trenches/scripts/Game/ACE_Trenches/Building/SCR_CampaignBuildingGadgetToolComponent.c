//------------------------------------------------------------------------------------------------
//! Extend E-tool for placing trenches outside of building editor mode
//! First use of GadgetActivate keybind will start the preview, a second will place it
modded class SCR_CampaignBuildingGadgetToolComponent : SCR_GadgetComponent
{
	[Attribute(defvalue: "{E14F00C2836CAB68}PrefabsEditable/Auto/Compositions/Misc/FreeRoamBuilding/E_ACE_DirtCover_01_long_v1.et", desc: "Prefab name for the buildable trench", params: "et")]
	protected ResourceName m_sACE_Trenches_TrenchPrefabName;
	
	protected CameraBase m_pACE_Trenches_CharacterCamera;
	protected IEntity m_pACE_Trenches_PreviewEntity;
	
	//------------------------------------------------------------------------------------------------
	//! Building tool out of hands -> end building mode.
	override void ToolToInventory()
	{
		super.ToolToInventory();
		ToggleActive(false);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Send request for placing trench 
	void ACE_Trenches_RequestPlace()
	{
		PlayerController playerController = GetGame().GetPlayerController();
		if (!playerController)
			return;
			
		SCR_CampaignBuildingNetworkComponent buildingNetworkComponent = SCR_CampaignBuildingNetworkComponent.Cast(playerController.FindComponent(SCR_CampaignBuildingNetworkComponent));
		if (!buildingNetworkComponent)
			return;
			
		vector transform[4];
		ACE_Trenches_GetPlacementTransform(transform);
		buildingNetworkComponent.ACE_Trenches_RequestPlace(m_sACE_Trenches_TrenchPrefabName, transform);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Creates preview entity
	protected void ACE_Trenches_StartPlacementPreview()
	{
		Resource res = Resource.Load(m_sACE_Trenches_TrenchPrefabName);
		if (!res)
			return;
		
		m_pACE_Trenches_CharacterCamera = GetGame().GetCameraManager().CurrentCamera();
		m_pACE_Trenches_PreviewEntity = SCR_PrefabPreviewEntity.SpawnPreviewFromPrefab(res, "SCR_PrefabPreviewEntity", null, null, "{58F07022C12D0CF5}Assets/Editor/PlacingPreview/Preview.emat");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Deletes preview entity
	protected void ACE_Trenches_EndPlacementPreview()
	{
		delete m_pACE_Trenches_PreviewEntity;
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
		transform[3] = m_CharacterOwner.GetOrigin() + 2.5 * vectorDir;
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
	}
	
	//------------------------------------------------------------------------------------------------
	//! Enable gadget to have on/off states
	override bool CanBeToggled()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Toggles building placement preview
	override void ToggleActive(bool state)
	{
		if (state)
			ACE_Trenches_StartPlacementPreview();
		else
			ACE_Trenches_EndPlacementPreview();
		
		m_bActivated = state;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Callback for GadgetActivate keybind
	override void ActivateAction()
	{
		super.ActivateAction();
		
		// Place object if on state is terminated via keybind
		if (m_bActivated)
			ACE_Trenches_RequestPlace();
		
		ToggleActive(!m_bActivated);
	}
}