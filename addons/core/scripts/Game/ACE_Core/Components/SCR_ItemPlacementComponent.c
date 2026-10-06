//------------------------------------------------------------------------------------------------
modded class SCR_ItemPlacementComponent : ScriptComponent
{
	protected static const float ACE_SURFACE_TRACER_LENGTH_M = 0.2;
	
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
	
	//------------------------------------------------------------------------------------------------
	//! Add check for entities that cannot be stacked
	override protected bool ValidateEntity(notnull IEntity entity)
	{
		if (!super.ValidateEntity(entity))
			return false;
		
		if (m_PlaceableItem.ACE_CanBeStacked())
			return true;
		
		return (entity.GetPrefabData() != m_PlaceableItem.GetOwner().GetPrefabData());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Validate surface property
	override void ValidatePlacement(vector up, IEntity tracedEntity, BaseWorld world, IEntity character, out ENotification cantPlaceReason)
	{
		super.ValidatePlacement(up, tracedEntity, world, character, cantPlaceReason);
		if (cantPlaceReason > 0)
			return;
		
		ACE_ESurfaceLabel label = m_PlaceableItem.ACE_RequiredSurfaceLabel();
		if (label == ACE_ESurfaceLabel.NONE)
			return;
		
		TraceParam params = new TraceParam();
		params.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
		params.Start = m_vCurrentMat[3] + 0.5 * ACE_SURFACE_TRACER_LENGTH_M * up;
		params.End = m_vCurrentMat[3] - 0.5 * ACE_SURFACE_TRACER_LENGTH_M * up;
		params.Include = tracedEntity;
		
		if (float.AlmostEqual(GetGame().GetWorld().TraceMove(params, null), 1) || !ACE_SurfaceHelper.HasLabel(params.SurfaceProps, label))
			cantPlaceReason = ENotification.PLACEABLE_ITEM_CANT_PLACE_GENERIC;
	}
}
