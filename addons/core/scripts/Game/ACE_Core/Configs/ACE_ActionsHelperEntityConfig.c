//----------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleResourceName("m_sRequiredBasePrefabName", true)]
class ACE_ActionsHelperEntityConfig
{
	[Attribute(desc: "Base prefab name of the entity", uiwidget: UIWidgets.ResourceNamePicker, params: "et")]
	protected ResourceName m_sRequiredBasePrefabName;
	
	[Attribute(desc: "Prefab name of the actions helper used for this entity", UIWidgets.ResourceNamePicker, params: "et")]
	protected ResourceName m_sHelperPrefabName;
	
	[Attribute(defvalue: "0 1.25 0", desc: "Offset for the helper prefab")]
	protected vector m_vOffset;
	
	protected typename m_tRequiredType;
	
	//----------------------------------------------------------------------------------------
	void Init()
	{
		Resource res = Resource.Load(m_sRequiredBasePrefabName);
		if (!res.IsValid())
			return;
		
		IEntitySource src = res.GetResource().ToEntitySource();
		if (!src)
			return;
		
		m_tRequiredType = src.GetClassName().ToType();
	}
	
	//----------------------------------------------------------------------------------------
	bool ValidateEntity(IEntity entity)
	{
		if (entity.Type() != m_tRequiredType)
			return false;
		
		EntityPrefabData prefabData = entity.GetPrefabData();
		if (!prefabData)
			return false;
		
		if (!SCR_BaseContainerTools.IsKindOf(prefabData.GetPrefabName(), m_sRequiredBasePrefabName))
			return false;
		
		return true;
	}
	
	//----------------------------------------------------------------------------------------
	//! Spawns actions helper entity for the corrsponding entity on the local player
	IEntity SpawnHelper(IEntity entity)
	{
		Resource res = Resource.Load(m_sHelperPrefabName);
		if (!res.IsValid())
			return null;
		
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		entity.GetWorldTransform(params.Transform);
		params.Transform[3] = params.Transform[3] + entity.VectorToParent(m_vOffset);
		
		ACE_ActionsHelperEntity helper = ACE_ActionsHelperEntity.Cast(GetGame().SpawnEntityPrefabLocal(res, entity.GetWorld(), params));
		if (!helper)
			return null;
		
		helper.SetAssociatedEntity(entity);
		return helper;
	}
}
