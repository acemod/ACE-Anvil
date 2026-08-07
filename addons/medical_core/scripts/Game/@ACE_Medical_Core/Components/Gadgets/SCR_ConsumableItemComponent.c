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
	
	protected const float ACE_MEDICAL_LITTER_TRACE_START_HEIGHT = 0.65; // Height in meters relative to patient to start trace
	protected const float ACE_MEDICAL_LITTER_MIN_WALL_DISTANCE = 0.25; // Minimum distance from walls in meters
	
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
		vector xzOffset = SCR_Math2D.GenerateRandomPointInRadius(m_fACE_Medical_MinLitterDistance, m_fACE_Medical_MaxLitterDistance, vector.Zero);
		// Trace walls
		TraceSphere traceParam = new TraceSphere();
		traceParam.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
		traceParam.TargetLayers = EPhysicsLayerPresets.Building;
		traceParam.Radius = ACE_MEDICAL_LITTER_MIN_WALL_DISTANCE;
		traceParam.Start = patientPos + ACE_MEDICAL_LITTER_TRACE_START_HEIGHT * vector.Up;
		traceParam.End = traceParam.Start + xzOffset;
		params.Transform[3] = traceParam.Start + GetGame().GetWorld().TraceMove(traceParam) * xzOffset;
		// Trace floor
		TraceParam traceParam2 = new TraceParam();
		traceParam2.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
		traceParam2.TargetLayers = EPhysicsLayerPresets.Building;
		SCR_TerrainHelper.SnapAndOrientToTerrain(params.Transform, trace: traceParam2);
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
		
		if (!Replication.IsServer() || !successful)
			return;
		
		// Check whether litter can be spawned
		ACE_Medical_Core_Settings settings = ACE_SettingsHelperT<ACE_Medical_Core_Settings>.GetModSettings();
		if (settings && !settings.m_bLitterEnabled)
			return;
				
		SCR_ConsumableItemComponentClass data = SCR_ConsumableItemComponentClass.Cast(GetComponentData(GetOwner()));
		if (!data || !data.ACE_Medical_ShouldSpawnLitterOnItemUseComplete())
			return;
		
		ChimeraCharacter patient = ChimeraCharacter.Cast(GetTargetCharacter());
		if (!patient)
			patient = m_CharacterOwner;
		
		if (!patient || patient.IsInVehicle())
			return;

		ACE_Medical_SpawnLitter(patient.GetOrigin());
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
