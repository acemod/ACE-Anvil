//------------------------------------------------------------------------------------------------
[EntityEditorProps(category: "GameScripted/Weapon/Sights", description: "", color: "0 0 255 255")]
modded class SCR_2DOpticsComponentClass : ScriptedSightsComponentClass
{
	[Attribute(defvalue: "30 30 0", desc: "Maximum values of zeroing in mrads. Horizontal is x, vertical is y.", category: "BaseSights")]
	protected vector m_fACE_MaxZeroingsMrad;
	
	[Attribute(defvalue: "-30 -30 0", desc: "Minimal values of zeroing in mrads. Horizontal is x, vertical is y.", category: "BaseSights")]
	protected vector m_fACE_MinZeroingsMrad;
	
	[Attribute(defvalue: "1.0", desc: "Factor for scaling effective elevation change. Can be used for fine-tuning accuracy.", category: "BaseSights")]
	protected float m_fACE_EffectiveVerticalZeroingScale;
	
	bool m_bACE_Initialized = false;
	bool m_bACE_HasZeroings = false;
	protected vector m_vACE_CameraAnglesOffsets;
			
	//------------------------------------------------------------------------------------------------
	//! Enforce limit for zeroing angles in mrads
	void ACE_LimitZeroings(inout float horizontal, inout float vertical)
	{
		horizontal = Math.Clamp(horizontal, m_fACE_MinZeroingsMrad[0], m_fACE_MaxZeroingsMrad[0]);
		vertical = Math.Clamp(vertical, m_fACE_MinZeroingsMrad[1], m_fACE_MaxZeroingsMrad[1]);
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_SetCameraAngleOffsets(vector offsets)
	{
		offsets[0] = offsets[0] * m_fACE_EffectiveVerticalZeroingScale;
		m_vACE_CameraAnglesOffsets = offsets;
	}
	
	//------------------------------------------------------------------------------------------------
	override vector GetCameraAngles()
	{
		return super.GetCameraAngles() + m_vACE_CameraAnglesOffsets;
	}
}

//------------------------------------------------------------------------------------------------
modded class SCR_2DOpticsComponent : ScriptedSightsComponent
{
	protected float m_fACE_HorizontalZeroing = 0.0;
	protected float m_fACE_VerticalZeroing = 0.0;
	protected IEntitySource m_ACE_ProjectileSource;
	protected float m_fACE_ProjectileInitSpeedCoef;
	protected float m_fACE_ProjectileInitSpeed;
	protected ProjectileMoveComponent m_ACE_DummyMoveComponent;
	
	//------------------------------------------------------------------------------------------------
	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);
		
		SCR_2DOpticsComponentClass data = SCR_2DOpticsComponentClass.Cast(GetComponentData(GetOwner()));
		if (!data || data.m_bACE_Initialized)
			return;
		
		array<vector> sightsRanges = ACE_GetSightsRanges();
		data.m_bACE_HasZeroings = (sightsRanges && sightsRanges.Count() > 1);
		data.m_bACE_Initialized = true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Compute offsets for current weapon
	override void OnSightADSActivated()
	{
		super.OnSightADSActivated();
		
		BaseWeaponComponent weaponComponent = ACE_GetWeaponComponent();
		if (!weaponComponent)
			return;
				
		IEntity weaponEntity = weaponComponent.GetOwner();
		if (!weaponEntity)
			return;
		
		BaseMuzzleComponent muzzle = weaponComponent.GetCurrentMuzzle();
		if (!muzzle)
			return;
		
		array<ResourceName> bulletNames = ACE_BulletTools.GetDefaultResourceNamesFromMuzzle(muzzle);
		if (bulletNames.IsEmpty())
			return;
		
		Resource res = Resource.Load(bulletNames[0]);
		if (!res.IsValid())
			return;
		
		IEntity dummy = GetGame().SpawnEntityPrefabLocal(res);
		if (!dummy)
			return;
		
		m_ACE_DummyMoveComponent = ProjectileMoveComponent.Cast(dummy.FindComponent(ProjectileMoveComponent));
		m_ACE_ProjectileSource = res.GetResource().ToBaseContainer();
		m_fACE_ProjectileInitSpeedCoef = muzzle.GetBulletInitSpeedCoef();
		m_fACE_ProjectileInitSpeed = m_fACE_ProjectileInitSpeedCoef * ACE_BulletTools.GetInitialSpeed(bulletNames[0]);
		
		// Reset camera angles offsets, as they are on the shared data component
		ACE_SetZeroings(m_fACE_HorizontalZeroing, m_fACE_VerticalZeroing, init: true);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnSightADSDeactivated()
	{
		super.OnSightADSDeactivated();
		
		if (m_ACE_DummyMoveComponent)
			SCR_EntityHelper.DeleteEntityAndChildren(m_ACE_DummyMoveComponent.GetParentProjectile());
	}
	
	//------------------------------------------------------------------------------------------------
	//! Changes horizontal and/or vertical zeroings by the given values in mrad
	void ACE_AdjustZeroings(float horizontalChange = 0.0, float verticalChange = 0.0)
	{		
		SCR_2DOpticsComponentClass data = SCR_2DOpticsComponentClass.Cast(GetComponentData(GetOwner()));
		if (!data)
			return;
		
		// Not available if scope has no zeroing in vanilla
		if (!data.m_bACE_HasZeroings)
			return;
		
		float horizontal;
		float vertical;
		ACE_GetZeroings(horizontal, vertical);
		horizontal += horizontalChange;
		vertical += verticalChange;
		ACE_SetZeroings(horizontal, vertical);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Set horizontal and vertical zeroings in mrad
	void ACE_SetZeroings(float horizontal, float vertical, bool init=false)
	{
		vector cameraAnglesOffsets;
		
		SCR_2DOpticsComponentClass data = SCR_2DOpticsComponentClass.Cast(GetComponentData(GetOwner()));
		if (!data)
			return;
		
		data.ACE_LimitZeroings(horizontal, vertical);
		m_fACE_HorizontalZeroing = horizontal;
		m_fACE_VerticalZeroing = vertical;
		
		if (init || IsSightADSActive())
		{
			cameraAnglesOffsets[0] = ACE_ComputeCameraPitchAngleOffset(vertical);
			cameraAnglesOffsets[1] = Math.RAD2DEG * SCR_Math.ConvertToRadians(-horizontal, SCR_EOpticsAngleUnits.MILLIRADIANS);
			data.ACE_SetCameraAngleOffsets(cameraAnglesOffsets);
		}
		
		SCR_WeaponInfo display = ACE_GetWeaponInfoDisplay();
		if (display)
			display.ACE_UpdateZeroingIndicator();
		
		if (!init)
			SCR_UISoundEntity.SoundEvent("ACE_SCOPES_SOUND_CLICK");
	}
		
	//------------------------------------------------------------------------------------------------
	//! Get horizontal and vertical zeroings in mrad
	void ACE_GetZeroings(out float horizontal, out float vertical)
	{
		horizontal = m_fACE_HorizontalZeroing;
		vertical = m_fACE_VerticalZeroing;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Compute the required camera pitch angle offset for the given vertical zeroing
	//! Assumes that zeroing is the elevation angle of the bore and transforms it into required elevation angle of sight
	//! \param[in] zeroing Vertical zeroing in milliradians
	//! \return pitch angle offset in degrees
	protected float ACE_ComputeCameraPitchAngleOffset(float zeroing)
	{
		float zeroRange = GetCurrentSightsRange()[1];
		float time;
		float zeroAngle = Math.Atan2(BallisticTable.GetHeightFromProjectileSource(zeroRange, time, m_ACE_ProjectileSource, m_fACE_ProjectileInitSpeedCoef), zeroRange);
		
		if (time < 0)
			return SCR_Math.ConvertToRadians(zeroing, SCR_EOpticsAngleUnits.MILLIRADIANS);
		
		float zeroSightToBoreAngle = ACE_ComputeSightToBoreAngle(zeroAngle, zeroRange);
		float targetAngle = zeroAngle + SCR_Math.ConvertToRadians(zeroing, SCR_EOpticsAngleUnits.MILLIRADIANS);
		
		vector targetResult = m_ACE_DummyMoveComponent.GetProjectileSimulationResult(
			vector.Zero, // initPosition
			m_fACE_ProjectileInitSpeed, // initSpeed
			Math.RAD2DEG * targetAngle, // initElevationAngle
			0, // initAzimuth
			vector.Zero, // windVelocity
			0 // targetHeight
		);
				
		float targetRange = targetResult[2];
		float targetSightToBoreAngle = ACE_ComputeSightToBoreAngle(targetAngle, targetRange);
		
		PrintFormat("|%1|%2|", zeroing, SCR_Math.ConvertFromRadians(targetSightToBoreAngle - zeroSightToBoreAngle, SCR_EOpticsAngleUnits.MILLIRADIANS));
		return -Math.RAD2DEG * (targetSightToBoreAngle - zeroSightToBoreAngle);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Compute angle between bore vector and target-aligned sight vector in radians
	//! \param[in] boreElevationAngle Target-aligned elevation angle of the bore in radians
	//! \param[in] range Range to the target in meters
	//! \return sights to bore angle in radians
	protected float ACE_ComputeSightToBoreAngle(float boreElevationAngle, float range)
	{
		vector sightTransform[4];
		GetSightsTransform(sightTransform, false);
		
		BaseWeaponComponent weaponComponent = ACE_GetWeaponComponent();
		if (!weaponComponent)
			return boreElevationAngle;
				
		IEntity weaponEntity = weaponComponent.GetOwner();
		if (!weaponEntity)
			return boreElevationAngle;
		
		Animation anim = weaponEntity.GetAnimation();
		vector chamberTransform[4];
		
		if (!anim.GetBoneMatrix(anim.GetBoneIndex("barrel_chamber"), chamberTransform))
			return boreElevationAngle;
		
		vector chamberPos = weaponEntity.CoordToParent(chamberTransform[3]);
		vector boreVector = weaponEntity.VectorToParent(chamberTransform[2]);
		vector sightOffset = sightTransform[3] - chamberPos;
		float sightOffsetLength = sightOffset.Length();
		float boreToSightOffsetAngle = ACE_Test.CosAngle(sightOffset, boreVector);
		
		return Math.Atan2(
			range * Math.Sin(boreElevationAngle) + sightOffsetLength * Math.Sin(boreToSightOffsetAngle),
			range * Math.Cos(boreElevationAngle) - sightOffsetLength * Math.Cos(boreToSightOffsetAngle)
		);
	}
	
	//------------------------------------------------------------------------------------------------
	protected SCR_WeaponInfo ACE_GetWeaponInfoDisplay()
	{
		SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
		if (!player)
			return null;
		
		SCR_BaseHUDComponent hudComponent = SCR_BaseHUDComponent.Cast(player.FindComponent(SCR_BaseHUDComponent));
		if (!hudComponent)
			return null;
		
		array<BaseInfoDisplay> displayInfos = {};
		int count = hudComponent.GetInfoDisplays(displayInfos);
		for (int i = 0; i < count; i++)
		{
			SCR_WeaponInfo current = SCR_WeaponInfo.Cast(displayInfos[i]);
			if (current)
				return current;
		}
		
		return null;
	}
	
	//------------------------------------------------------------------------------------------------
	array<vector> ACE_GetSightsRanges()
	{
		IEntityComponentSource source = GetComponentSource(GetOwner());
		if (!source)
			return null;
		
		BaseContainerList sightsRangeSources = source.GetObjectArray("SightsRanges");
		if (!sightsRangeSources)
			return null;
		
		array<vector> ranges = {};
		ranges.Reserve(sightsRangeSources.Count());
		
		for (int i = 0; i < sightsRangeSources.Count(); ++i)
		{
			BaseContainer sightsRangeSource = sightsRangeSources.Get(i);
			if (!sightsRangeSource)
				continue;
			
			vector range;
			sightsRangeSource.Get("Range", range);
			ranges.Insert(range);
		}
		
		return ranges;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return weapon the sight is attached to
	BaseWeaponComponent ACE_GetWeaponComponent()
	{
		IEntity parent = GetOwner();

		BaseWeaponComponent weapon;
		while (parent)
		{
			weapon = BaseWeaponComponent.Cast(parent.FindComponent(BaseWeaponComponent));
			if (weapon)
				break;
			
			parent = parent.GetParent();
		}
		
		return weapon;
	}
}
