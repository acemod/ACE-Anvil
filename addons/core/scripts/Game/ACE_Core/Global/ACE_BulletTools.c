//------------------------------------------------------------------------------------------------
class ACE_BulletTools
{
	static const float MAX_SIMULATION_TIME = 60; // [s]
	static const float SIMULATION_TIME_STEP = 0.005; // [s]
	
	//------------------------------------------------------------------------------------------------
	//! Return default bullet resource names of a muzzle
	static array<ResourceName> GetDefaultResourceNamesFromMuzzle(BaseMuzzleComponent muzzle)
	{
		ResourceName magazineOrProjectilePrefabName = muzzle.GetDefaultMagazineOrProjectileName();
		if (magazineOrProjectilePrefabName.IsEmpty())
		{
			BaseMagazineComponent magazine = muzzle.GetMagazine();
			if (!magazine)
				return {};
			
			magazineOrProjectilePrefabName = magazine.GetOwner().GetPrefabData().GetPrefabName();
		}
		
		array<ResourceName> bulletPrefabNames = GetResourceNamesFromMagazine(magazineOrProjectilePrefabName);
		if (bulletPrefabNames)
			return bulletPrefabNames;
		
		return {magazineOrProjectilePrefabName};
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return all bullet resource names of a magazine
	static array<ResourceName> GetResourceNamesFromMagazine(ResourceName magazinePrefabName)
	{
		Resource magazineRes = Resource.Load(magazinePrefabName);
		if (!magazineRes.IsValid())
			return null;
		
		BaseContainer mazagineSource = magazineRes.GetResource().ToBaseContainer();
		if (!mazagineSource)
			return null;
		
		IEntityComponentSource magazineComponentSource = ACE_BaseContainerTools.FindComponentSource(mazagineSource, BaseMagazineComponent);
		if (!magazineComponentSource)
			return null;
		
		ResourceName ammoConfigName;
		magazineComponentSource.Get("AmmoConfig", ammoConfigName);
		
		Resource ammoConfigRes = Resource.Load(ammoConfigName);
		if (!ammoConfigRes.IsValid())
			return null;
		
		BaseContainer ammonConfigSrc = ammoConfigRes.GetResource().ToBaseContainer();
		if (!ammonConfigSrc)
			return null;
		
		array<ResourceName> ammoNames = {};
		ammonConfigSrc.Get("AmmoResourceArray", ammoNames);
		return ammoNames;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return mass of bullet
	static float GetMass(ResourceName bulletPrefabName)
	{
		return GetFloatProperty(bulletPrefabName, "Mass");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return mass of bullet
	static float GetMass(IEntitySource bulletSource)
	{
		return GetFloatProperty(bulletSource, "Mass");
	}
	
	//------------------------------------------------------------------------------------------------
	static float GetAirDrag(ResourceName bulletPrefabName)
	{
		return GetFloatProperty(bulletPrefabName, "AirDrag");
	}
	
	//------------------------------------------------------------------------------------------------
	static float GetAirDrag(IEntitySource bulletSource)
	{
		return GetFloatProperty(bulletSource, "AirDrag");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return initial speed of bullet
	static float GetInitialSpeed(ResourceName bulletPrefabName)
	{
		return GetFloatProperty(bulletPrefabName, "InitSpeed");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return initial speed of bullet
	static float GetInitialSpeed(IEntitySource bulletSource)
	{
		return GetFloatProperty(bulletSource, "InitSpeed");
	}
	
	//------------------------------------------------------------------------------------------------
	protected static float GetFloatProperty(ResourceName bulletPrefabName, string propertyName)
	{
		Resource bulletRes = Resource.Load(bulletPrefabName);
		if (!bulletRes.IsValid())
			return 0;
		
		IEntitySource bulletSource = bulletRes.GetResource().ToEntitySource();
		if (!bulletSource)
			return 0;

		return GetFloatProperty(bulletSource, propertyName);
	}
	
	//------------------------------------------------------------------------------------------------
	protected static float GetFloatProperty(IEntitySource bulletSource, string propertyName)
	{
		IEntityComponentSource shellMovementSource = ACE_BaseContainerTools.FindComponentSource(bulletSource, ShellMoveComponent);
		if (!shellMovementSource)
			return 0;
		
		float value;
		shellMovementSource.Get(propertyName, value);
		return value;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Computes lateral wind drift for the given flight time
	//! \param[in] bulletSource Source container of the bullet
	//! \param[in] initialSpeed Initial speed in m/s
	//! \param[in] windSpeed Crosswind speed in m/s
	//! \param[in] time Duration of the flight to simulate in seconds
	//! \return lateral wind drift in meters
	static float ComputeLateralDrift(IEntitySource bulletSource, float initialSpeed, float windSpeed, float time)
	{
		float dragCoef = GetAirDrag(bulletSource) / GetMass(bulletSource);
		vector velBullet = {initialSpeed, 0, 0};
		vector velWind = {0, 0, windSpeed};
		float drift = 0;
		float timer = 0;
		int nSteps = Math.Ceil(time / SIMULATION_TIME_STEP);
		
		for (int i = 0; i < nSteps; ++i)
		{
			float dt = Math.Min(SIMULATION_TIME_STEP, time - timer);
			if (dt <= 0.0)
				break;
			
			vector velRel = velBullet - velWind;
			velBullet -= dt * dragCoef * velRel.Length() * velRel;
			drift += dt * velBullet[2];
			timer += dt;
		}
		
		return drift;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Computes required elevation angle for given range. Returns negative time if not reachable.
	//! \param[in] bulletSource Source container of the bullet
	//! \param[in] range Target range in meters
	//! \param[in] initialSpeed Initial speed in m/s
	//! \param[out] time Duration of the flight in seconds
	//! \return elevation angle in radians
	static float ComputeElevationAngleForRange(IEntitySource bulletSource, float range, float initialSpeed, out float time)
	{
		time = -1;
		ACE_BulletTools_RangeErrorForElevationAngle f = ACE_BulletTools_RangeErrorForElevationAngle(bulletSource, initialSpeed, range);
		float guess = 0.5 * Math.Asin(Physics.STANDARD_GRAVITY * range / initialSpeed / initialSpeed); // Use vacuum solution as guess
		ACE_MathTools_RootResult<float> result = ACE_MathTools.Secant(f, guess, guess + 0.001, xtol: 0.00001);
		
		if (!result.m_bConverged)
			result = ACE_MathTools.Bisect(f, guess, 0.030, xtol: 0.00001);
		if (!result.m_bConverged)
			return 0;
		
		time = f.GetFlightTime();
		return result.m_fRoot;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Computes range for given elevation angle. Returns negative time if simulation times out.
	//! \param[in] bulletSource Source container of the bullet
	//! \param[in] elevationAngle Target elevation angle in radians
	//! \param[in] initialSpeed Initial speed in m/s
	//! \param[out] time Duration of the flight in seconds
	//! \return range in meters
	static float ComputeRangeForElevationAngle(IEntitySource bulletSource, float elevationAngle, float initialSpeed, out float time)
	{
		float dragCoef = ACE_BulletTools.GetAirDrag(bulletSource) / ACE_BulletTools.GetMass(bulletSource);
		vector vel = initialSpeed * Vector(Math.Cos(elevationAngle), Math.Sin(elevationAngle), 0);
		vector pos = vector.Zero;
		vector prevPos;
		time = 0;
		
		int nSteps = Math.Ceil(MAX_SIMULATION_TIME / SIMULATION_TIME_STEP);
		
		for (int i = 0; i < nSteps; ++i)
		{
			prevPos = pos;
			vel += SIMULATION_TIME_STEP * (Physics.VGravity - dragCoef * vel.Length() * vel);
			pos += SIMULATION_TIME_STEP * vel;
			time += SIMULATION_TIME_STEP;
			
			if (pos[1] <= 0.0 && prevPos[1] >= 0.0)
			{
				// Get final result from interpolation
				float progress = Math.InverseLerp(prevPos[1], pos[1], 0);
				time -= SIMULATION_TIME_STEP * (1 - progress);
				return Math.Lerp(prevPos[0], pos[0], progress);
			}
		}
		
		time = -1;
		return 0;
	}
}

//------------------------------------------------------------------------------------------------
//! Computes signed error for given initial elevation to match target range
class ACE_BulletTools_RangeErrorForElevationAngle : ACE_MathTools_FunctionBase<float, float>
{
	protected IEntitySource m_BulletSource;
	protected float m_fInitalSpeed;
	protected float m_fTargetRange;
	protected float m_fTime;
	
	//------------------------------------------------------------------------------------------------
	void ACE_BulletTools_RangeErrorForElevationAngle(IEntitySource bulletSource, float initalSpeed, float targetRange)
	{
		m_BulletSource = bulletSource;
		m_fInitalSpeed = initalSpeed;
		m_fTargetRange = targetRange;
	}
	
	//------------------------------------------------------------------------------------------------
	//! \param[in] x Initial elevation in radians
	override float Eval(float x)
	{
		float range = ACE_BulletTools.ComputeRangeForElevationAngle(m_BulletSource, x, m_fInitalSpeed, m_fTime);
		return m_fTargetRange - range;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetFlightTime()
	{
		return m_fTime;
	}
}
