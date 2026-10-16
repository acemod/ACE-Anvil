//------------------------------------------------------------------------------------------------
class ACE_Medical_InspectionOverlaySystem : GameSystem
{
	[Attribute(defvalue: "0.5", desc: "Timeout between overlay state updates in seconds.")]
	protected float m_fUpdateTimeout;

	protected float m_fUpdateTimer;
	protected ChimeraCharacter m_Target;
	protected ACE_Medical_InspectionOverlayComponentClass m_OverlayData;
	protected SCR_CharacterDamageManagerComponent m_TargetDamageManager;
	protected ECharacterHitZoneGroup m_eHighlightedGroup = ECharacterHitZoneGroup.VIRTUAL;
	protected IEntity m_OverlayEntity;
	protected string m_sOverlayMaterialOptions;
	protected ref SCR_InventoryHitZonePointUI m_HitZonePointUI = new SCR_InventoryHitZonePointUI();

	//------------------------------------------------------------------------------------------------
	static ACE_Medical_InspectionOverlaySystem GetInstance(ChimeraWorld world)
	{
		return ACE_Medical_InspectionOverlaySystem.Cast(world.FindSystem(ACE_Medical_InspectionOverlaySystem));
	}

	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(false)
			.SetUnique(true)
			.SetLocation(WorldSystemLocation.Client)
			.AddPoint(WorldSystemPoint.Frame);
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		super.OnInit();
		Enable(false);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnStarted()
	{
		super.OnStarted();
		m_fUpdateTimer = m_fUpdateTimeout;
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnUpdatePoint(WorldUpdatePointArgs args)
	{
		super.OnUpdatePoint(args);

		m_fUpdateTimer -= args.GetTimeSliceSeconds();
		if (m_fUpdateTimer > 0)
			return;
		
		UpdateOverlayEntity();
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnCleanup()
	{
		DeleteOverlay();
		super.OnCleanup();
	}

	//------------------------------------------------------------------------------------------------
	void GenerateOverlay(ChimeraCharacter target)
	{
		DeleteOverlay();
		
		m_Target = target;
		if (!m_Target)
			return;
		
		ACE_Medical_InspectionOverlayComponent overlayComponent = ACE_Medical_InspectionOverlayComponent.Cast(m_Target.FindComponent(ACE_Medical_InspectionOverlayComponent));
		if (!overlayComponent)
			return;
		
		m_OverlayData = ACE_Medical_InspectionOverlayComponentClass.Cast(overlayComponent.GetComponentData(target));
		if (!m_OverlayData)
			return;
		
		m_TargetDamageManager = SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!m_TargetDamageManager)
			return;
		
		m_OverlayEntity = SpawnOverlayEntity();
		Enable(true);
	}

	//------------------------------------------------------------------------------------------------
	void SetHighlightedHitZoneGroup(ECharacterHitZoneGroup hitZoneGroup)
	{
		if (m_eHighlightedGroup == hitZoneGroup)
			return;

		m_eHighlightedGroup = hitZoneGroup;
		UpdateOverlayEntity();
	}
	
	//------------------------------------------------------------------------------------------------
	void DeleteOverlay()
	{
		SCR_EntityHelper.DeleteEntityAndChildren(m_OverlayEntity);
		m_OverlayEntity = null;
		m_Target = null;
		m_OverlayData = null;
		m_TargetDamageManager = null;
		m_eHighlightedGroup = ECharacterHitZoneGroup.VIRTUAL;
		m_fUpdateTimer = 0;
		m_sOverlayMaterialOptions = "";
		Enable(false);
	}
	
	//------------------------------------------------------------------------------------------------
	protected IEntity SpawnOverlayEntity()
	{
		if (!m_Target)
			return null;
		
		Resource res = Resource.Load(m_OverlayData.GetBodyMeshName());
		if (!res || !res.IsValid())
			return null;
		
		VObject mesh = res.GetResource().ToVObject();
		if (!mesh)
			return null;
		
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		m_Target.GetWorldTransform(params.Transform);

		IEntity overlayEntity = GetGame().SpawnEntity(GenericEntity, m_Target.GetWorld(), params);
		if (!overlayEntity)
			return null;

		vector transform[4];
		Math3D.MatrixIdentity4(transform);
		m_Target.AddChild(overlayEntity, -1);
		overlayEntity.SetLocalTransform(transform);
		
		m_sOverlayMaterialOptions = CompileOverlayMaterialOptions();
		overlayEntity.SetObject(mesh, m_sOverlayMaterialOptions);
		overlayEntity.SetVComponentFlags(VCFlags.INHERIT_PARENT_SKELETON);
		return overlayEntity;
	}

	//------------------------------------------------------------------------------------------------
	protected void UpdateOverlayEntity()
	{
		m_fUpdateTimer = m_fUpdateTimeout;
		
		if (!m_Target || !m_OverlayEntity)
		{
			DeleteOverlay();
			return;
		}
		
		string materialOptions = CompileOverlayMaterialOptions();
		if (m_sOverlayMaterialOptions == materialOptions)
			return;
		
		m_sOverlayMaterialOptions = materialOptions;
		m_OverlayEntity.SetObject(m_OverlayEntity.GetVObject(), m_sOverlayMaterialOptions);
		m_OverlayEntity.SetVComponentFlags(VCFlags.INHERIT_PARENT_SKELETON);
	}
	
	//------------------------------------------------------------------------------------------------
	protected string CompileOverlayMaterialOptions()
	{
		string options;
		foreach (ECharacterHitZoneGroup hitZoneGroup : m_TargetDamageManager.LIMB_GROUPS)
		{
			if (hitZoneGroup == ECharacterHitZoneGroup.VIRTUAL)
				continue;

			ResourceName material = GetHitZoneGroupMaterial(hitZoneGroup, hitZoneGroup == m_eHighlightedGroup);
			string slotName = typename.EnumToString(ECharacterHitZoneGroup, hitZoneGroup);
			options += string.Format("$remap '%1' '%2';", slotName, SCR_FileIOHelper.GetShortResourceName(material));
		}
		
		return options;
	}

	//------------------------------------------------------------------------------------------------
	protected ResourceName GetHitZoneGroupMaterial(ECharacterHitZoneGroup hitZoneGroup, bool highlighted = false)
	{
		float bleedingRate = m_TargetDamageManager.GetGroupDamageOverTime(hitZoneGroup, EDamageType.BLEEDING);
		if (m_TargetDamageManager.GetGroupTourniquetted(hitZoneGroup))
			bleedingRate *= m_TargetDamageManager.GetTourniquetStrengthMultiplier();

		if (bleedingRate > 0)
		{
			if (bleedingRate >= m_HitZonePointUI.m_fHighBleedingThreshold)
				return m_OverlayData.GetHeavyBleedingMaterial(highlighted);

			return m_OverlayData.GetModerateBleedingMaterial(highlighted);
		}

		if (m_TargetDamageManager.GetGroupHealthScaled(hitZoneGroup) < 1.0)
			return m_OverlayData.GetImprovingMaterial(highlighted);
		else
			return m_OverlayData.GetUninjuredMaterial(highlighted);
	}
}
