class ACE_VisualisedBallisticConfig : SCR_VisualisedBallisticConfig
{
	protected static const ref array<float> WIND_SPEEDS = {4, 6, 8};
	protected static const float MAX_DROP = -30.0;
	
	//------------------------------------------------------------------------------------------------
	void ACE_VisualisedBallisticConfig(ResourceName projectilePrefab, SCR_EOpticsAngleUnits unitType = SCR_EOpticsAngleUnits.MILLIRADIANS)
	{
		m_sProjectilePrefab = projectilePrefab;
		m_sDisplayedText = FilePath.StripExtension(FilePath.StripPath(projectilePrefab));
		m_eUnitType = unitType;
		m_fProjectileInitSpeedCoef = m_eUnitType;  // Used as storage for comparison on SCR_BallisticData
		m_iRangeStep = 50;
		m_iMinRange = 100;
		m_iMaxRange = 1500;
		m_iElevationChangeDownRange = 50;
		m_fStandardDispersion = 100;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GenerateBallisticData()
	{
		if (VerifyDataExistence(m_iBallisticDataId))
			return true;
		
		float initialSpeed = ACE_BulletTools.GetInitialSpeed(m_sProjectilePrefab);
		IEntity dummy = GetGame().SpawnEntityPrefabLocal(Resource.Load(m_sProjectilePrefab), GetGame().GetWorld());
		ProjectileMoveComponent moveComponent = ProjectileMoveComponent.Cast(dummy.FindComponent(ProjectileMoveComponent));

		array<ref array<float>> ballisticValues = {};
		float drop = -0.1;
		float zeroDrop;

		for (int range = m_iMinRange; range <= m_iMaxRange; range += m_iRangeStep)
		{
			array<float> row = {range};
			row.Resize(2 + WIND_SPEEDS.Count());
			float firstDrop = 0.0;
			bool firstDropSet = false;
				
			foreach (int i, float windSpeed : WIND_SPEEDS)
			{
				float windage;
				
				if (!ComputeDropAndWindage(moveComponent, range, initialSpeed, windSpeed, drop, windage))
					break;
				
				if (!firstDropSet)
				{
					if (drop < MAX_DROP)
					{
						// Replace invalid drops by zeroDrop to get them replaced by dash
						firstDrop = zeroDrop;
						break;
					}
					
					firstDrop = drop;
					firstDropSet = true;
				}
				
				row[2 + i] = ACE_Math.Round(windage, 1);
			}
			
			if (ballisticValues.IsEmpty())
				zeroDrop = firstDrop;
			else
				row[1] = ACE_Math.Round(firstDrop - zeroDrop, 1);
			
			ballisticValues.Insert(row);
		}
		
		SCR_EntityHelper.DeleteEntityAndChildren(dummy);

		SCR_BallisticData ballisticData = new SCR_BallisticData(ballisticValues, m_sProjectilePrefab, m_bDirectFireMode, m_iRangeStep, m_fProjectileInitSpeedCoef);
		if (!SCR_BallisticData.s_aBallistics)
			SCR_BallisticData.s_aBallistics = {};

		SCR_BallisticData.s_aBallistics.Insert(ballisticData);
		m_iBallisticDataId = SCR_BallisticData.s_aBallistics.Count() - 1;
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Optimize drop with secant method to match targetRange
	//! Return drop and windage in angle
	protected bool ComputeDropAndWindage(ProjectileMoveComponent moveComponent, float targetRange, float initialSpeed, float windSpeed, inout float drop, out float windage)
	{
		ACE_Ballistics_DropResidual fn = new ACE_Ballistics_DropResidual(moveComponent, targetRange, initialSpeed, windSpeed);
		ACE_MathTools_RootResult<float> solution = ACE_MathTools.Secant(fn, drop + 0.1, drop - 0.1, ftol: 0.1, xtol: 0.0);
		drop = MilliradiansToUnit(solution.m_fRoot);
		windage = MilliradiansToUnit(fn.GetWindageResult());
		return solution.m_bConverged;
	}
	
	//------------------------------------------------------------------------------------------------
	protected vector ComputeProjectileSimulationResult(ProjectileMoveComponent moveComponent, float initialSpeed, float windSpeed, float drop)
	{
		return moveComponent.GetProjectileSimulationResult(
			vector.Zero, // initPosition
			initialSpeed, // initSpeed
			Math.RAD2DEG * 1e-3 * -drop, // initElevationAngle
			0, // initAzimuth
			{windSpeed, 0, 0}, // windVelocity
			0, // targetHeight
			10, // maxSimulationTime
		);
	}
	
	//------------------------------------------------------------------------------------------------
	protected float MilliradiansToUnit(float value)
	{
		if (m_eUnitType == SCR_EOpticsAngleUnits.MILLIRADIANS)
			return value;
		
		return SCR_Math.ConvertFromRadians(1e-3 * value, m_eUnitType);
	}
}

//------------------------------------------------------------------------------------------------
class ACE_Ballistics_DropResidual : ACE_MathTools_FunctionBase<float, float>
{
	protected ProjectileMoveComponent m_MoveComponent;
	protected float m_fTargetRange;
	protected float m_fInitialSpeed;
	protected vector m_vWindVelocity;
	protected vector m_fOffset;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Ballistics_DropResidual(ProjectileMoveComponent moveComponent, float targetRange, float initialSpeed, float windSpeed)
	{
		m_fTargetRange = targetRange;
		m_MoveComponent = moveComponent;
		m_fInitialSpeed = initialSpeed;
		m_vWindVelocity = {windSpeed, 0, 0};
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return residual between computed range for the drop x in mrad and the targetRange
	override float Eval(float x)
	{
		m_fOffset = m_MoveComponent.GetProjectileSimulationResult(
			vector.Zero, // initPosition
			m_fInitialSpeed, // initSpeed
			Math.RAD2DEG * 1e-3 * -x, // initElevationAngle
			0, // initAzimuth
			m_vWindVelocity, // windVelocity
			0, // targetHeight
			10, // maxSimulationTime
		);
		return m_fOffset[2] - m_fTargetRange;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetWindageResult()
	{
		return 1e3 * Math.Atan2(m_fOffset[0], m_fOffset[2]);
	}
}
