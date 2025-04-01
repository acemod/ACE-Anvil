//------------------------------------------------------------------------------------------------
modded class SCR_ItemPlacementComponent : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	//! Add handling for SCR_EPlacementType.ACE_XYZ_UP, which forces the item to be placed upright
	protected override void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
		
		if (m_PlaceableItem.GetPlacementType() != SCR_EPlacementType.ACE_XYZ_UP)
			return;
		
		CameraManager cameraManager = GetGame().GetCameraManager();
		if (!cameraManager)
			return;

		CameraBase currentCamera = cameraManager.CurrentCamera();
		if (!currentCamera)
			return;

		vector cameraMat[4];
		currentCamera.GetTransform(cameraMat);

		// Reuse the implementation of XYZ placement, but correct the angles afterwards
		m_eCantPlaceReason = 0;
		UseXYZPlacement(owner, m_PlaceableItem.GetMaxPlacementDistance(), cameraMat);
		Math3D.AnglesToMatrix(Vector(m_vCurrentMat[2].ToYaw(), 0, 0), m_vCurrentMat);
		m_PreviewEntity.SetTransform(m_vCurrentMat);
		m_PreviewEntity.Update();
		
		if (m_eCantPlaceReason == 0)
			SCR_Global.SetMaterial(m_PreviewEntity, m_sCanBuildMaterial);
		else if (m_eCantPlaceReason == ENotification.PLACEABLE_ITEM_CANT_PLACE_DISTANCE)
			SCR_Global.SetMaterial(m_PreviewEntity, m_sTransparentMaterial);
		else
			SCR_Global.SetMaterial(m_PreviewEntity, m_sCannotBuildMaterial);
	}
}
