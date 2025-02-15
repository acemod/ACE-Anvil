//------------------------------------------------------------------------------------------------
class ACE_AnimationTools
{
	protected static ref ACE_AnimationHelpersConfig s_AnimationHelpersConfig;
	protected static const ResourceName ANIMATION_HELPERS_CONFIG_NAME = "{49EB6E775F6E8519}Configs/ACE/ACE_AnimationHelpers.conf";
	
	//------------------------------------------------------------------------------------------------
	protected static void Init()
	{
		s_AnimationHelpersConfig = SCR_ConfigHelperT<ACE_AnimationHelpersConfig>.GetConfigObject(ANIMATION_HELPERS_CONFIG_NAME);
	}
	
	//------------------------------------------------------------------------------------------------
	static ACE_AnimationHelperCompartment AnimateWithHelperCompartment(ACE_EAnimationHelperID helperID, notnull IEntity performer)
	{
		vector transform[4];
		performer.GetWorldTransform(transform);
		return AnimateWithHelperCompartment(helperID, performer, transform);
	}
	
	//------------------------------------------------------------------------------------------------
	static ACE_AnimationHelperCompartment AnimateWithHelperCompartment(ACE_EAnimationHelperID helperID, notnull IEntity performer, vector transform[4])
	{
		if (!s_AnimationHelpersConfig)
			Init();
		
		Resource res = Resource.Load(s_AnimationHelpersConfig.GetPrefabName(helperID));
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
