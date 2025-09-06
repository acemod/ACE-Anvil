//------------------------------------------------------------------------------------------------
class ACE_Overheating_SmokeEffectComponentClass : MuzzleEffectComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_Overheating_SmokeEffectComponent : MuzzleEffectComponent
{
	[Attribute(defvalue: "{5EF5A6DBC1EA1B1D}Particles/Weapon/ACE_Overheating_MuzzleSmoke.ptc", uiwidget: UIWidgets.ResourceNamePicker)]
	protected ResourceName m_sMuzzleSmokeEffectName;
	
	[Attribute(defvalue: "{5EF5A6DBC1EA1B1D}Particles/Weapon/ACE_Overheating_MuzzleSmoke.ptc", uiwidget: UIWidgets.ResourceNamePicker)]
	protected ResourceName m_sBarrelSurfaceSmokeEffectName;
	
	[Attribute()]
	protected ref array<string> m_aTemperatureDependentEmitterNames;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Birth rate vs temperature [K]", params: "1300 100 0 0")]
	protected ref Curve m_cBirthRateTemperatureCurve;
	protected float m_fBirthRate;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Birth rate RND vs temperature [K]", params: "1300 100 0 0")]
	protected ref Curve m_cBirthRateRndTemperatureCurve;
	protected float m_fBirthRateRnd;
	
	// To Do: Fix name
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle lifetime multiplier vs temperature [K]", params: "1300 4 0 0")]
	protected ref Curve m_cLifetimeMultiplierMultiplierTemperatureCurve;
	protected float m_fLifetime;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle lifetime RND vs temperature [K]", params: "1300 4 0 0")]
	protected ref Curve m_cLifetimeRndMultiplierTemperatureCurve;
	protected float m_fLifetimeRnd;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle size multiplier vs temperature [K]", params: "1300 1 0 0")]
	protected ref Curve m_cSizeMultiplierTemperatureCurve;
	protected float m_fSize;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle size RND vs temperature [K]", params: "1300 1 0 0")]
	protected ref Curve m_cSizeRndTemperatureCurve;
	protected float m_fSizeRnd;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle velocity vs temperature [K]", params: "1300 3 0 0")]
	protected ref Curve m_cVelocityMultiplierTemperatureCurve;
	protected float m_fVelocity;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle velocity RND vs temperature [K]", params: "1300 3 0 0")]
	protected ref Curve m_cVelocityRndTemperatureCurve;
	protected float m_fVelocityRnd;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle emitting time vs temperature [K]", params: "1300 30 0 0")]
	protected ref Curve m_cEmittingTimeTemperatureCurve;
	protected float m_fEmittingTime;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Scripted particle emitting time [s] vs temperature [K]", params: "1300 30 0 0")]
	protected ref Curve m_cScriptedEmittingTimeTemperatureCurve;
	protected float m_fScriptedEmittingTimeMS;
	
	protected ACE_Overheating_BarrelComponent m_pBarrel;
	protected static ACE_Overheating_Settings s_pSettings;
	protected ParticleEffectEntity m_pMuzzleEffect;
	
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
		
		UpdateParams(ACE_PhysicalConstants.STANDARD_AMBIENT_TEMPERATURE);
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateParams(float temperature)
	{
		m_fBirthRate = Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cBirthRateTemperatureCurve)[1];
		m_fBirthRateRnd = Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cBirthRateRndTemperatureCurve)[1];
		m_fLifetime = Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cLifetimeMultiplierMultiplierTemperatureCurve)[1];
		m_fLifetimeRnd = Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cLifetimeRndMultiplierTemperatureCurve)[1];
		m_fSize = Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cSizeMultiplierTemperatureCurve)[1];
		m_fSizeRnd = Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cSizeRndTemperatureCurve)[1];
		m_fVelocity = Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cVelocityMultiplierTemperatureCurve)[1];
		m_fVelocityRnd = Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cVelocityRndTemperatureCurve)[1];
		m_fEmittingTime = Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cEmittingTimeTemperatureCurve)[1];
		m_fScriptedEmittingTimeMS = 1000 * Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cScriptedEmittingTimeTemperatureCurve)[1];
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnFired(IEntity effectEntity, BaseMuzzleComponent muzzle, IEntity projectileEntity)
	{
		super.OnFired(effectEntity, muzzle, projectileEntity);
		
		if (!m_pBarrel || m_pBarrel.GetBarrelTemperature() < s_pSettings.m_fMinSmokeTemperature)
			return;
		
		if (!m_pMuzzleEffect)
		{
			IEntity weapon = muzzle.GetOwner();
			Animation weaponAnim = weapon.GetAnimation();
			ParticleEffectEntitySpawnParams spawnParams = new ParticleEffectEntitySpawnParams();
			spawnParams.Parent = weapon;
			spawnParams.PivotID = weaponAnim.GetBoneIndex("barrel_muzzle");
			spawnParams.DeleteWhenStopped = false;
			m_pMuzzleEffect = ParticleEffectEntity.SpawnParticleEffect(m_sMuzzleSmokeEffectName, spawnParams);
		}
		
		GetGame().GetCallqueue().Remove(m_pMuzzleEffect.StopEmission);
		m_pMuzzleEffect.Play();
			
		
		Particles particles = m_pMuzzleEffect.GetParticles();
		array<string> allEmitterNames = {};
		particles.GetEmitterNames(allEmitterNames);

		foreach (int i, string emitterName : allEmitterNames)
		{
			if (!m_aTemperatureDependentEmitterNames.Contains(emitterName))
				continue;
			
			particles.SetParam(i, EmitterParam.BIRTH_RATE, m_fBirthRate);
			particles.SetParam(i, EmitterParam.BIRTH_RATE_RND, m_fBirthRateRnd);
			particles.SetParam(i, EmitterParam.LIFETIME, m_fLifetime);
			particles.SetParam(i, EmitterParam.LIFETIME_RND, m_fLifetimeRnd);
			particles.SetParam(i, EmitterParam.SIZE, m_fSize);
			particles.SetParam(i, EmitterParam.SIZE_RND, m_fSizeRnd);
			particles.SetParam(i, EmitterParam.VELOCITY, m_fVelocity);
			particles.SetParam(i, EmitterParam.VELOCITY_RND, m_fVelocityRnd);
			particles.SetParam(i, EmitterParam.EMITTING_TIME, m_fEmittingTime);
		}
		
		GetGame().GetCallqueue().CallLater(m_pMuzzleEffect.StopEmission, m_fScriptedEmittingTimeMS, false);
		
		/*
		ParticleEffectEntitySpawnParams spawnParams;
		Particles particles;
		array<string> allEmitterNames = {};
		
		
		spawnParams = new ParticleEffectEntitySpawnParams();
		spawnParams.Parent = weapon;
		spawnParams.PivotID = weaponAnim.GetBoneIndex("barrel_muzzle");
		
		vector muzzleTransform[4];
		weaponAnim.GetBoneMatrix(weaponAnim.GetBoneIndex("barrel_muzzle"), muzzleTransform);
		vector chamberTransform[4];
		weaponAnim.GetBoneMatrix(weaponAnim.GetBoneIndex("barrel_chamber"), chamberTransform);
		
		spawnParams.Transform[3] = 0.5 * (chamberTransform - muzzleTransform);
		ParticleEffectEntity barrelEffect = ParticleEffectEntity.SpawnParticleEffect(m_sBarrelSurfaceSmokeEffectName, spawnParams);
		particles = barrelEffect.GetParticles();
		allEmitterNames.Clear();
		particles.GetEmitterNames(allEmitterNames);
		
		foreach (int i, string emitterName : allEmitterNames)
		{
			if (!m_aTemperatureDependentEmitterNames.Contains(emitterName))
				continue;
			
			particles.SetParam(i, EmitterParam.BIRTH_RATE, m_fBirthRate);
			particles.SetParam(i, EmitterParam.BIRTH_RATE_RND, m_fBirthRateRnd);
			particles.SetParam(i, EmitterParam.LIFETIME, m_fLifetime);
			particles.SetParam(i, EmitterParam.LIFETIME_RND, m_fLifetimeRnd);
			particles.SetParam(i, EmitterParam.SIZE, m_fSize);
			particles.SetParam(i, EmitterParam.SIZE_RND, m_fSizeRnd);
			particles.SetParam(i, EmitterParam.VELOCITY, m_fVelocity);
			particles.SetParam(i, EmitterParam.VELOCITY_RND, m_fVelocityRnd);
			particles.SetParam(i, EmitterParam.EMITTING_TIME, m_fEmittingTime);
		}
		*/
	}
	
	//------------------------------------------------------------------------------------------------
	void ~ACE_Overheating_SmokeEffectComponent()
	{
		if (m_pMuzzleEffect)
			SCR_EntityHelper.DeleteEntityAndChildren(m_pMuzzleEffect);
	}
}
