//----------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true)]
class ACE_Chopping_Config
{
	[Attribute(desc: "Maps plant base prefab to its corresponding chopping helper entity")]
	protected ref array<ref ACE_Chopping_ConfigEntry> m_aEntries;
	
	protected ref map<ResourceName, ACE_Chopping_ConfigEntry> m_mResourceNameCache = new map<ResourceName, ACE_Chopping_ConfigEntry>();
	
	//----------------------------------------------------------------------------------------
	//! Spawns chopping helper entity for the corrsponding plant on the local player
	IEntity SpawnHelper(IEntity plant)
	{
		ACE_Chopping_ConfigEntry entry = GetConfigEntry(plant);
		if (!entry)
			return null;

		Resource res = Resource.Load(entry.m_sHelperPrefabName);
		if (!res.IsValid())
			return null;
				
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		plant.GetWorldTransform(params.Transform);
		params.Transform[3] = params.Transform[3] + entry.m_vOffset;
				
		ACE_Chopping_HelperEntity helper = ACE_Chopping_HelperEntity.Cast(GetGame().SpawnEntityPrefabLocal(res, plant.GetWorld(), params));
		if (!helper)
			return null;
				
		helper.SetAssociatedPlant(plant);
		return helper;
	}
	
	//----------------------------------------------------------------------------------------
	protected ACE_Chopping_ConfigEntry GetConfigEntry(IEntity plant)
	{
		EntityPrefabData prefabData = plant.GetPrefabData();
		if (!prefabData)
			return null;
		
		ResourceName resName = prefabData.GetPrefabName();
		
		ACE_Chopping_ConfigEntry cachedEntry;
		if (m_mResourceNameCache.Find(resName, cachedEntry))
			return cachedEntry;
		
		foreach (ACE_Chopping_ConfigEntry entry : m_aEntries)
		{
			if (SCR_BaseContainerTools.IsKindOf(resName, entry.m_sPlantPrefabName))
			{
				m_mResourceNameCache.Insert(resName, entry);
				return entry;
			}
		}
		
		return null;
	}
}

//----------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerCustomTitleResourceName("m_sPlantPrefabName", true)]
class ACE_Chopping_ConfigEntry
{
	[Attribute(desc: "Base prefab name of the plant", params: "et")]
	ResourceName m_sPlantPrefabName;
	
	[Attribute(desc: "Prefab name of the chopping helper used for this plant", params: "et")]
	ResourceName m_sHelperPrefabName;
	
	[Attribute(defvalue: "0 1.25 0", desc: "Offset for the helper prefab")]
	vector m_vOffset;
}
