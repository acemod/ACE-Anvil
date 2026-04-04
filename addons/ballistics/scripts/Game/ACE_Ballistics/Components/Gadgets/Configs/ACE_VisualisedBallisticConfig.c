class ACE_VisualisedBallisticConfig : SCR_VisualisedBallisticConfig
{
	protected float m_fInitSpeedCoef;
	protected float m_fDefaultZeroingRange;
	
	protected static const float WIND_SPEED = 4.0;
	protected static const float MIN_DROP = -30.0;
	
	//------------------------------------------------------------------------------------------------
	void ACE_VisualisedBallisticConfig(ResourceName projectilePrefab, float initSpeedCoef = 1.0, float defaultZeroingRange = 100.0, SCR_EOpticsAngleUnits unitType = SCR_EOpticsAngleUnits.MILLIRADIANS)
	{
		m_sProjectilePrefab = projectilePrefab;
		m_sDisplayedText = FilePath.StripExtension(FilePath.StripPath(projectilePrefab));
		float initialSpeed = initSpeedCoef * ACE_BulletTools.GetInitialSpeed(projectilePrefab);
		m_sDisplayedText += string.Format(" (%1 m/s)", Math.Round(initialSpeed));
		m_eUnitType = unitType;
		m_fProjectileInitSpeedCoef = initSpeedCoef;
		m_iRangeStep = 50;
		m_iMinRange = 100;
		m_iMaxRange = 1500;
		m_iElevationChangeDownRange = 50;
		m_fDefaultZeroingRange = defaultZeroingRange;
		m_fStandardDispersion = defaultZeroingRange;
		
		m_sLayoutName = "{94BEF98329CC0B81}UI/layouts/Gadgets/BallisticTable/ACE_BallisticTable_Mrads.layout";
		m_sRowPrefab = "{F75FE2331AF70BF8}UI/layouts/Gadgets/BallisticTable/ContentRowLayout.layout";
		m_sCellPrefab = "{801F5CED215A1CFF}UI/layouts/Gadgets/BallisticTable/Content.layout";
		m_aGridFillWeights = {1, 1, 1, 1, 1};
		m_sAverageDispersionFormat = "%1 m";
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GenerateBallisticData()
	{
		if (VerifyDataExistence(m_iBallisticDataId))
			return true;
		
		float initialSpeed = m_fProjectileInitSpeedCoef * ACE_BulletTools.GetInitialSpeed(m_sProjectilePrefab);
		IEntity dummy = GetGame().SpawnEntityPrefabLocal(Resource.Load(m_sProjectilePrefab), GetGame().GetWorld());
		ProjectileMoveComponent moveComponent = ProjectileMoveComponent.Cast(dummy.FindComponent(ProjectileMoveComponent));

		array<ref array<float>> ballisticValues = {};
		float time;
		float zeroDrop = ComputeProjectileDrop(moveComponent, m_fDefaultZeroingRange, time);

		for (int range = m_iMinRange; range <= m_iMaxRange; range += m_iRangeStep)
		{
			array<float> row = {range};
			row.Resize(3);
			
			float drop = ComputeProjectileDrop(moveComponent, range, time);
			
			if (drop < MIN_DROP)
				break;
			
			row[1] = drop;
			float windage = ComputeProjectileWindage(moveComponent, initialSpeed, WIND_SPEED, range, drop, time);
			row[2] = ACE_Math.Round(windage, 1);
			ballisticValues.Insert(row);
		}
		
		// Shift drops by default zeroing drop
		for (int i = 0; i < ballisticValues.Count(); ++i)
		{
			ballisticValues[i][1] = ACE_Math.Round(ballisticValues[i][1] - zeroDrop, 1);
		}
		
		SCR_EntityHelper.DeleteEntityAndChildren(dummy);

		SCR_BallisticData ballisticData = new SCR_BallisticData(ballisticValues, m_sProjectilePrefab, m_bDirectFireMode, m_iRangeStep, m_fProjectileInitSpeedCoef);
		ballisticData.ACE_SetUnitType(m_eUnitType);
		ballisticData.ACE_SetDefaultZeroingRange(m_fDefaultZeroingRange);
		
		if (!SCR_BallisticData.s_aBallistics)
			SCR_BallisticData.s_aBallistics = {};

		SCR_BallisticData.s_aBallistics.Insert(ballisticData);
		m_iBallisticDataId = SCR_BallisticData.s_aBallistics.Count() - 1;
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool VerifyDataExistence(inout int id = -1)
	{
		if (!super.VerifyDataExistence(id))
			return false;
		
		SCR_BallisticData ballisticData = SCR_BallisticData.s_aBallistics[id];
		return (ballisticData.ACE_GetUnitType() == m_eUnitType) && (ballisticData.ACE_GetDefaultZeroingRange() == m_fDefaultZeroingRange);
	}
	
	protected float ComputeProjectileDrop(ProjectileMoveComponent moveComponent, float range, float time)
	{
		float height = BallisticTable.GetHeightFromProjectile(range, time, moveComponent.GetParentProjectile(), m_fProjectileInitSpeedCoef);
		return -SCR_Math.ConvertFromRadians(Math.Atan2(height, range), m_eUnitType);
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeProjectileWindage(ProjectileMoveComponent moveComponent, float initialSpeed, float windSpeed, float range, float drop, float time)
	{
		vector offset = moveComponent.GetProjectileSimulationResult(
			vector.Zero, // initPosition
			initialSpeed, // initSpeed
			-Math.RAD2DEG * SCR_Math.ConvertToRadians(drop, m_eUnitType), // initElevationAngle
			0, // initAzimuth
			{windSpeed, 0, 0}, // windVelocity
			0, // targetHeight
			maxSimulationTime: time, // maxSimulationTime
		);
		return SCR_Math.ConvertFromRadians(Math.Atan2(offset[0], range), m_eUnitType);
	}
}
