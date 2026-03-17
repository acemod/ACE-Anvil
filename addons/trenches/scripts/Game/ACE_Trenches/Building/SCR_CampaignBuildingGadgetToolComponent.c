//------------------------------------------------------------------------------------------------
//! Extend E-tool for placing trenches outside of building editor mode
//! First use of GadgetActivate keybind will start the preview, a second will place it
modded class SCR_CampaignBuildingGadgetToolComponent : SCR_GadgetComponent
{
    [Attribute(defvalue: "{E14F00C2836CAB68}PrefabsEditable/Auto/Compositions/Misc/FreeRoamBuilding/E_ACE_DirtCover_01_long_v1.et",
               desc: "Prefab name for the buildable trench", params: "et")]
    protected ResourceName m_sACE_Trenches_TrenchPrefabName;

    [Attribute(defvalue: "2.5", desc: "Distance in meters from player for placement preview")]
    protected float m_sACE_Trenches_PlacementDistanceM;

    protected CameraBase m_pACE_Trenches_CharacterCamera;
    protected IEntity m_pACE_Trenches_PreviewEntity;
    protected bool m_bACE_InHand = false;

    //------------------------------------------------------------------------------------------------
    //! Building tool taken to hand
    override void ToolToHand()
    {
        super.ToolToHand();

        // ToolToHand gets triggered twice when changing between crouch and prone stance. We therefore
        // skip the execution if it was already called
        if (m_bACE_InHand)
            return;

        m_bACE_InHand = true;
        GetGame().GetInputManager().AddActionListener("CharacterInspect", EActionTrigger.DOWN, ToogleActiveAction);
    }

    //------------------------------------------------------------------------------------------------
    //! Building tool out of hands -> end building mode.
    override void ToolToInventory()
    {
        super.ToolToInventory();

        if (!m_bACE_InHand)
            return;

        m_bACE_InHand = false;
        ToggleActive(false);
        GetGame().GetInputManager().RemoveActionListener("CharacterInspect", EActionTrigger.DOWN, ToogleActiveAction);
    }

    //------------------------------------------------------------------------------------------------
    //! Send request for placing trench
    void ACE_Trenches_RequestPlace()
    {
        PlayerController playerController = GetGame().GetPlayerController();
        if (!playerController)
            return;

        SCR_CampaignBuildingNetworkComponent buildingNetworkComponent =
            SCR_CampaignBuildingNetworkComponent.Cast(playerController.FindComponent(SCR_CampaignBuildingNetworkComponent));
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
        m_pACE_Trenches_PreviewEntity = SCR_PrefabPreviewEntity.SpawnPreviewFromPrefab(res, "SCR_PrefabPreviewEntity", null, null,
                                                                                       "{58F07022C12D0CF5}Assets/Editor/PlacingPreview/Preview.emat");
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
        if (m_bActivated == state)
            return;

        if (state)
        {
            ACE_Trenches_StartPlacementPreview();
            SCR_UISoundEntity.SoundEvent("SOUND_E_PLACE_GSTART");
        }
        else
        {
            ACE_Trenches_EndPlacementPreview();
            SCR_UISoundEntity.SoundEvent("SOUND_HUD_GADGET_CANCEL");
        }

        m_bActivated = state;
    }

    //------------------------------------------------------------------------------------------------
    //! Callback for GadgetActivate keybind
    //! Place object if placement mode is active
    override void ActivateAction()
    {
        if (!m_bActivated)
            return;

        ACE_Trenches_RequestPlace();
        SCR_UISoundEntity.SoundEvent("SOUND_E_ACE_TRENCH_BUILD");
    }

    //------------------------------------------------------------------------------------------------
    //! Callback for CharacterInspect keybind
    void ToogleActiveAction()
    {
        ToggleActive(!m_bActivated);
    }
}
