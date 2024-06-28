/*class RON_UserActionPrefabReplaceSpawnerClass : ScriptedUserActionClass
{

}*/

class RON_UserActionPrefabReplaceSpawner : ScriptedUserAction
{	
	
	[Attribute("", UIWidgets.ResourcePickerThumbnail, "Prefab to Spawn")]
	protected ResourceName  m_prefabPath;
	
	[Attribute ("", UIWidgets.Font, "Set outName")]
	protected string actionName;
	
	[Attribute("", UIWidgets.Auto)]
	ref SCR_AudioSourceConfiguration m_AudioSourceConfiguration;
	
	[Attribute("", UIWidgets.Coords)]
	private vector m_vSoundOffset;
	
	private AudioHandle m_AudioHandle = AudioHandle.Invalid;
	
	protected static ref ScriptInvokerInt s_onActionComplete;
	
	//------------------------------------------------------------------------------------------------

	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{

		Resource loadedResource = Resource.Load(m_prefabPath);
		if (!loadedResource)
			return;
		
		 IEntity spawnedPrefab = IEntity.Cast(GetGame().SpawnEntityPrefab(loadedResource));    
        if(!spawnedPrefab)
            return;
        
        vector WorldTransfrom[4];
        pOwnerEntity.GetWorldTransform(WorldTransfrom);
        spawnedPrefab.SetWorldTransform(WorldTransfrom);
        spawnedPrefab.Update();
        SCR_EntityHelper.DeleteEntityAndChildren(pOwnerEntity);
		
		if (s_onActionComplete)
		s_onActionComplete.Invoke(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity));
		
		SCR_SoundManagerEntity soundManagerEntity = GetGame().GetSoundManagerEntity();
		if (!soundManagerEntity)
			return;
		
		if (!m_AudioSourceConfiguration || !m_AudioSourceConfiguration.IsValid())
			return;
		
		SCR_AudioSource audioSource = soundManagerEntity.CreateAudioSource(spawnedPrefab, m_AudioSourceConfiguration);
		if (!audioSource)
			return;
		
		vector mat[4];
		spawnedPrefab.GetTransform(mat);
		mat[3] = spawnedPrefab.CoordToParent(m_vSoundOffset);
		
		soundManagerEntity.PlayAudioSource(audioSource, mat);
	
		m_AudioHandle = audioSource.m_AudioHandle;
	}
	
	//------------------------------------------------------------------------------------------------
	static ScriptInvokerInt GetOnActionComplete()
	{
		if (!s_onActionComplete)
			s_onActionComplete = new ScriptInvokerInt();
		
		return s_onActionComplete;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = actionName;
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	void ~RON_UserActionPrefabReplaceSpawner()
	{
		
	}
};