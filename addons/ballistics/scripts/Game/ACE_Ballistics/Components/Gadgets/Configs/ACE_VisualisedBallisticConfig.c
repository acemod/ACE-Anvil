class ACE_VisualisedBallisticConfig : SCR_VisualisedBallisticConfig
{
	protected float m_fInitSpeedCoef;
	protected float m_fDefaultZeroingRange;
	
	protected static const ref array<float> WIND_SPEEDS = {4, 6, 8};
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
		float zeroDrop = 0.0;

		for (int range = m_iMinRange; range <= m_iMaxRange; range += m_iRangeStep)
		{
			array<float> row = {range};
			row.Resize(2 + WIND_SPEEDS.Count());
			
			float time;
			float height = BallisticTable.GetHeightFromProjectile(range, time, dummy, m_fProjectileInitSpeedCoef);
			float drop = -SCR_Math.ConvertFromRadians(Math.Atan2(height, range), m_eUnitType);
			
			if (drop < MIN_DROP)
				break;
			
			row[1] = drop;
			
			if (range == m_fDefaultZeroingRange)
				zeroDrop = drop;
						
			foreach (int i, float windSpeed : WIND_SPEEDS)
			{
				float xOffset = ComputeProjectileWindageOffset(moveComponent, initialSpeed, windSpeed, drop, time);
				float windage = SCR_Math.ConvertFromRadians(Math.Atan2(xOffset, range), m_eUnitType);
				row[2 + i] = ACE_Math.Round(windage, 1);
			}
			
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
		return (ballisticData.ACE_GetUnitType() == m_eUnitType);
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeProjectileWindageOffset(ProjectileMoveComponent moveComponent, float initialSpeed, float windSpeed, float drop, float time)
	{
		vector offset = moveComponent.GetProjectileSimulationResult(
			vector.Zero, // initPosition
			initialSpeed, // initSpeed
			-Math.RAD2DEG * SCR_Math.ConvertToRadians(drop, m_eUnitType), // initElevationAngle
			0, // initAzimuth
			{windSpeed, 0, 0}, // windVelocity
			0, // targetHeight
			time, // maxSimulationTime
		);
		return offset[0];
	}
}
