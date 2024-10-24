//------------------------------------------------------------------------------------------------
modded class SCR_CampaignBuildingLayoutComponent : ScriptComponent
{
	//------------------------------------------------------------------------------------------------
	//! Enable vertical snap to geometries
	override void SpawnPreview()
	{
		if (m_PreviewEntity)
			return;
		
		super.SpawnPreview();
		if (!m_PreviewEntity)
			return;
		
		IEntity ent = GetOwner().GetRootParent();
		if (!ent)
			return;

		ent.RemoveChild(m_PreviewEntity);
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		ent.GetWorldTransform(spawnParams.Transform);
		m_PreviewEntity.SetPreviewTransform(spawnParams.Transform, EEditorTransformVertical.TERRAIN | EEditorTransformVertical.GEOMETRY);
		ent.AddChild(m_PreviewEntity, -1, EAddChildFlags.RECALC_LOCAL_TRANSFORM);
		m_PreviewEntity.Update();
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Trenches_SetToBuildValue(int value)
	{
		m_iToBuildValue = value;
		Replication.BumpMe();
	}
}
