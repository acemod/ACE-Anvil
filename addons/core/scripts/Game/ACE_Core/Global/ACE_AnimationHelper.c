//------------------------------------------------------------------------------------------------
class ACE_AnimationTools
{
	//------------------------------------------------------------------------------------------------
	static ACE_AnimationHelperCompartment AnimateWithHelperCompartment(notnull IEntity performer, ResourceName helperPrefabName)
	{
		vector transform[4];
		performer.GetWorldTransform(transform);
		return AnimateWithHelperCompartment(performer, transform, helperPrefabName);
	}
	
	//------------------------------------------------------------------------------------------------
	static ACE_AnimationHelperCompartment AnimateWithHelperCompartment(notnull IEntity performer, vector transform[4], ResourceName helperPrefabName)
	{
		Resource res = Resource.Load(helperPrefabName);
		if (!res.IsValid())
			return null;
		
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform = transform;
		
		ACE_AnimationHelperCompartment helper = ACE_AnimationHelperCompartment.Cast(GetGame().SpawnEntityPrefab(res, null, params));
		if (!helper)
			return null;
		
		helper.Init(performer);
		return helper;
	}
	
	//------------------------------------------------------------------------------------------------
	static void TerminateHelperCompartment(notnull IEntity performer, EGetOutType getOutType = EGetOutType.ANIMATED)
	{
		ACE_AnimationHelperCompartment helper = ACE_AnimationHelperCompartment.Cast(performer.GetParent());
		if (helper)
			helper.Terminate(getOutType);
	}
}
