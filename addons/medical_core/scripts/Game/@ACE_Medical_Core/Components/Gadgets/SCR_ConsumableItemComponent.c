//------------------------------------------------------------------------------------------------
[EntityEditorProps(category: "GameScripted/Gadgets", description: "Consumable gadget")]
modded class SCR_ConsumableItemComponentClass : SCR_GadgetComponentClass
{
	[Attribute(desc: "Configs for litter to be spawned after successful usage.", category: "Litter")]
	protected ref array<ref ACE_PrefabVariantsConfig> m_aACE_Medical_LitterPrefabConfigs;
	
	[Attribute(defvalue: "1", desc: "Minimum distance in meters for litter to spawn from patient.", category: "Litter")]
	protected float m_fACE_Medical_MinLitterDistance;
	
	[Attribute(defvalue: "2", desc: "Maximum distance in meters for litter to spawn from patient.", category: "Litter")]
	protected float m_fACE_Medical_MaxLitterDistance;
	
	[Attribute(defvalue: "true", desc: "Whether litter should automatically be spawned when item has been used.", category: "Litter")]
	protected bool m_bACE_Medical_SpawnLitterOnItemUseComplete;
	
	//------------------------------------------------------------------------------------------------
	array<ref ACE_PrefabVariantsConfig> ACE_Medical_GetLitterPrefabConfigs()
	{
		return m_aACE_Medical_LitterPrefabConfigs;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Generate random spawn params for litter based on the patient's position
	EntitySpawnParams ACE_Medical_GenerateLitterSpawnParams(vector patientPos)
	{
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		Math3D.AnglesToMatrix(Vector(Math.RandomFloat(0, 360), 0, 0), params.Transform);
		params.Transform[3] = SCR_Math2D.GenerateRandomPointInRadius(m_fACE_Medical_MinLitterDistance, m_fACE_Medical_MaxLitterDistance, patientPos);
		SCR_TerrainHelper.SnapAndOrientToTerrain(params.Transform);
		return params;
	}
	
	//------------------------------------------------------------------------------------------------
	bool ACE_Medical_ShouldSpawnLitterOnItemUseComplete()
	{
		return m_bACE_Medical_SpawnLitterOnItemUseComplete;
	}
}

//------------------------------------------------------------------------------------------------
modded class SCR_ConsumableItemComponent : SCR_GadgetComponent
{
	//------------------------------------------------------------------------------------------------
	//! Spawn litter if enabled
	override protected void OnApplyToCharacter(IEntity item, bool successful, ItemUseParameters animParams)
	{
		super.OnApplyToCharacter(item, successful, animParams);
		
		if (!Replication.IsServer())
			return;
		
		// Check whether litter can be spawned
		ACE_Medical_Core_Settings settings = ACE_SettingsHelperT<ACE_Medical_Core_Settings>.GetModSettings();
		if (settings && !settings.m_bLitterEnabled)
			return;
				
		SCR_ConsumableItemComponentClass data = SCR_ConsumableItemComponentClass.Cast(GetComponentData(GetOwner()));
		if (!data || !data.ACE_Medical_ShouldSpawnLitterOnItemUseComplete())
			return;
		
		IEntity target = GetTargetCharacter();
		if (!target)
			target = m_CharacterOwner;

		ACE_Medical_SpawnLitter(target.GetOrigin());
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_SpawnLitter(vector patientPos)
	{
		SCR_ConsumableItemComponentClass data = SCR_ConsumableItemComponentClass.Cast(GetComponentData(GetOwner()));
		if (!data)
			return;
		
		array<ref ACE_PrefabVariantsConfig> configs = data.ACE_Medical_GetLitterPrefabConfigs();
		if (configs.IsEmpty())
			return;
		
		foreach (ACE_PrefabVariantsConfig config : configs)
		{
			Resource res = Resource.Load(config.GetRandomVariant());
			if (!res.IsValid())
				continue;
			
			EntitySpawnParams params = data.ACE_Medical_GenerateLitterSpawnParams(patientPos);
			GetGame().SpawnEntityPrefab(res, GetOwner().GetWorld(), params);
		}
	}
}
