//------------------------------------------------------------------------------------------------
class ACE_Overheating_SmokeEffectComponentClass : MuzzleEffectComponentClass
{
	[Attribute(defvalue: "{5EF5A6DBC1EA1B1D}Particles/Weapon/ACE_Overheating_MuzzleSmoke.ptc", uiwidget: UIWidgets.ResourceNamePicker)]
	protected ResourceName m_sMuzzleSmokeEffectName;
	
	[Attribute(defvalue: "{5EF5A6DBC1EA1B1D}Particles/Weapon/ACE_Overheating_MuzzleSmoke.ptc", uiwidget: UIWidgets.ResourceNamePicker)]
	protected ResourceName m_sBarrelSurfaceSmokeEffectName;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Birth rate vs temperature [K]", params: "1300 100 0 0")]
	protected ref Curve m_cBirthRateTemperatureCurve;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle lifetime multiplier vs temperature [K]", params: "1300 4 0 0")]
	protected ref Curve m_cLifetimeTemperatureCurve;
	
	[Attribute(defvalue: "1", desc: "Particle lifetime Rnd", params: "0 10")]
	protected float m_fLifetimeRndScale;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle size multiplier vs temperature [K]", params: "1300 1 0 0")]
	protected ref Curve m_cSizeTemperatureCurve;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle velocity vs temperature [K]", params: "1300 3 0 0")]
	protected ref Curve m_cVelocityTemperatureCurve;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle emitting time vs temperature [K]", params: "1300 30 0 0")]
	protected ref Curve m_cEmittingTimeTemperatureCurve;
	
	//------------------------------------------------------------------------------------------------
	ResourceName GetMuzzleSmokeEffectName()
	{
		return m_sMuzzleSmokeEffectName;
	}
	
	//------------------------------------------------------------------------------------------------
	ResourceName GetBarrelSurfaceSmokeEffectName()
	{
		return m_sBarrelSurfaceSmokeEffectName;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetLifetimeRndScale()
	{
		return m_fLifetimeRndScale;
	}
	
	//------------------------------------------------------------------------------------------------
	float ComputeBirthRate(float temperature)
	{
		return Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cBirthRateTemperatureCurve)[1];
	}
	
	//------------------------------------------------------------------------------------------------
	float ComputeLifetime(float temperature)
	{
		return Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cLifetimeTemperatureCurve)[1];
	}
	
	//------------------------------------------------------------------------------------------------
	float ComputeSize(float temperature)
	{
		return Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cSizeTemperatureCurve)[1];
	}
	
	//------------------------------------------------------------------------------------------------
	float ComputeVelocity(float temperature)
	{
		return Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cVelocityTemperatureCurve)[1];
	}
	
	//------------------------------------------------------------------------------------------------
	float ComputeEmittingTime(float temperature)
	{
		return 1000 * Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cEmittingTimeTemperatureCurve)[1];
	}
}

//------------------------------------------------------------------------------------------------
class ACE_Overheating_SmokeEffectComponent : MuzzleEffectComponent
{
	protected ACE_Overheating_SmokeEffectComponentClass m_pData;
	protected float m_fBirthRate;
	protected float m_fLifetime;
	protected float m_fSize;
	protected float m_fVelocity;
	protected float m_fEmittingTimeMS;
	
	protected ACE_Overheating_BarrelComponent m_pBarrel;
	protected static ACE_Overheating_Settings s_pSettings;
	protected ParticleEffectEntity m_pMuzzleEffect;
	protected ParticleEffectEntity m_pBarrelSurfaceEffect;
	
	//------------------------------------------------------------------------------------------------
	static ACE_Overheating_SmokeEffectComponent FromMuzzle(BaseMuzzleComponent muzzle)
	{
		if (!muzzle)
			return null;
		
		return ACE_Overheating_SmokeEffectComponent.Cast(muzzle.FindComponent(ACE_Overheating_SmokeEffectComponent));
	}
		
	//------------------------------------------------------------------------------------------------
	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);
		m_pBarrel = ACE_Overheating_BarrelComponent.FromWeapon(owner);
		
		if (!s_pSettings)
			s_pSettings = ACE_SettingsHelperT<ACE_Overheating_Settings>.GetModSettings();
		
		m_pData = ACE_Overheating_SmokeEffectComponentClass.Cast(GetComponentData(owner));
		UpdateParams(ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE);
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateParams(float temperature)
	{
		m_fBirthRate = m_pData.ComputeBirthRate(temperature);
		m_fLifetime = m_pData.ComputeLifetime(temperature);
		m_fSize = m_pData.ComputeSize(temperature);
		m_fVelocity = m_pData.ComputeVelocity(temperature);
		m_fEmittingTimeMS = m_pData.ComputeEmittingTime(temperature);
	}
		
	//------------------------------------------------------------------------------------------------
	override void OnFired(IEntity effectEntity, BaseMuzzleComponent muzzle, IEntity projectileEntity)
	{
		super.OnFired(effectEntity, muzzle, projectileEntity);
		
		if (!m_pBarrel || m_pBarrel.GetBarrelTemperature() < s_pSettings.m_fMinSmokeTemperature)
			return;
		
		if (!m_pMuzzleEffect)
			InitEffects(muzzle);
		
		GetGame().GetCallqueue().Remove(StopEffects);
		StartEffects();
		ApplyParamsToEffects();
		GetGame().GetCallqueue().CallLater(StopEffects, m_fEmittingTimeMS);
	}
	
	//------------------------------------------------------------------------------------------------
	void InitEffects(BaseMuzzleComponent muzzle)
	{
		IEntity weapon = muzzle.GetOwner();
		Animation weaponAnim = weapon.GetAnimation();
		ParticleEffectEntitySpawnParams spawnParams = new ParticleEffectEntitySpawnParams();
		spawnParams.Parent = weapon;
		int pivotID = weaponAnim.GetBoneIndex("barrel_muzzle");
		spawnParams.PivotID = pivotID;
		spawnParams.DeleteWhenStopped = false;
		m_pMuzzleEffect = ParticleEffectEntity.SpawnParticleEffect(m_pData.GetMuzzleSmokeEffectName(), spawnParams);
		
		spawnParams = new ParticleEffectEntitySpawnParams();
		spawnParams.Parent = weapon;
		spawnParams.PivotID = pivotID;
		spawnParams.DeleteWhenStopped = false;
		vector muzzleTransform[4];
		weaponAnim.GetBoneMatrix(pivotID, muzzleTransform);
		vector chamberTransform[4];
		weaponAnim.GetBoneMatrix(weaponAnim.GetBoneIndex("barrel_chamber"), chamberTransform);
		spawnParams.Transform[3] = 0.5 * (chamberTransform[3] - muzzleTransform[3]);
		m_pBarrelSurfaceEffect = ParticleEffectEntity.SpawnParticleEffect(m_pData.GetBarrelSurfaceSmokeEffectName(), spawnParams);
	}
	
	//------------------------------------------------------------------------------------------------
	void ApplyParamsToEffects()
	{
		array<Particles> particlesList = {m_pMuzzleEffect.GetParticles(), m_pBarrelSurfaceEffect.GetParticles()};
		
		foreach (Particles particles : particlesList)
		{
			particles.SetParam(-1, EmitterParam.BIRTH_RATE, m_fBirthRate);
			particles.SetParam(-1, EmitterParam.LIFETIME, m_fLifetime);
			particles.SetParam(-1, EmitterParam.LIFETIME_RND, m_pData.GetLifetimeRndScale() * m_fLifetime);
			particles.SetParam(-1, EmitterParam.SIZE, m_fSize);
			particles.SetParam(-1, EmitterParam.VELOCITY, m_fVelocity);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void StartEffects()
	{
		m_pMuzzleEffect.Play();
		m_pBarrelSurfaceEffect.Play();
	}
	
	//------------------------------------------------------------------------------------------------
	void StopEffects()
	{
		m_pMuzzleEffect.StopEmission();
		m_pBarrelSurfaceEffect.StopEmission();
	}
	
	//------------------------------------------------------------------------------------------------
	void ~ACE_Overheating_SmokeEffectComponent()
	{
		SCR_EntityHelper.DeleteEntityAndChildren(m_pMuzzleEffect);
		SCR_EntityHelper.DeleteEntityAndChildren(m_pBarrelSurfaceEffect);			
	}
}
