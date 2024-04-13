//----------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true)]
class ACE_Chopping_Config
{
	[Attribute(desc: "Maps plant base prefab to its corresponding chopping helper entity")]
	protected ref array<ref ACE_Chopping_ConfigEntry> m_aEntries;
	
	//----------------------------------------------------------------------------------------
	//! Spawns chopping helper entity for the corrsponding plant on the local player
	IEntity CreateHelper(IEntity plant)
	{
		EntityPrefabData prefabData = plant.GetPrefabData();
		if (!prefabData)
			return null;
		
		BaseContainer container = prefabData.GetPrefab();
		if (!container)
			return null;
		
		foreach (ACE_Chopping_ConfigEntry entry: m_aEntries)
		{
			if (SCR_BaseContainerTools.IsKindOf(container, entry.m_sPlantPrefabName))
			{
				Resource res = Resource.Load(entry.m_sHelperPrefabName);
				if (!res.IsValid())
					continue;
				
				EntitySpawnParams params = new EntitySpawnParams();
				params.TransformMode = ETransformMode.WORLD;
				plant.GetWorldTransform(params.Transform);
				params.Transform[3] = params.Transform[3] + entry.m_vOffset;
				
				ACE_Chopping_HelperEntity helper = ACE_Chopping_HelperEntity.Cast(GetGame().SpawnEntityPrefabLocal(res, plant.GetWorld(), params));
				if (!helper)
					continue;
				
				helper.SetAssociatedPlant(plant);
				return helper;
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
