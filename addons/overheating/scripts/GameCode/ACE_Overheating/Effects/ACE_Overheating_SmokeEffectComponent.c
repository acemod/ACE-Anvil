//------------------------------------------------------------------------------------------------
class ACE_Overheating_SmokeEffectComponentClass : MuzzleEffectComponentClass
{
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
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle emitting time vs temperature [K]", params: "1300 60 0 0")]
	protected ref Curve m_cEmittingTimeTemperatureCurve;
	
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
		return Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cEmittingTimeTemperatureCurve)[1];
	}
}

//------------------------------------------------------------------------------------------------
class ACE_Overheating_SmokeEffectComponent : MuzzleEffectComponent
{
	[Attribute()]
	protected ref array<ref ACE_Overheating_SmokeEffect> m_aSomkeEffects;
	
	protected ACE_Overheating_SmokeEffectComponentClass m_pData;
	protected float m_fBirthRate;
	protected float m_fLifetime;
	protected float m_fSize;
	protected float m_fVelocity;
	protected float m_fEmittingTimeMS;
	
	protected ACE_Overheating_BarrelComponent m_pBarrel;
	protected static ACE_Overheating_Settings s_pSettings;
	bool m_bInitDone = false;
	
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
		m_fEmittingTimeMS = 1000 * m_pData.ComputeEmittingTime(temperature);
	}
		
	//------------------------------------------------------------------------------------------------
	override void OnFired(IEntity effectEntity, BaseMuzzleComponent muzzle, IEntity projectileEntity)
	{
		super.OnFired(effectEntity, muzzle, projectileEntity);
		
		if (!m_pBarrel || m_pBarrel.GetBarrelTemperature() < s_pSettings.m_fMinSmokeTemperature)
			return;
		
		if (!m_bInitDone)
			InitEffects(muzzle.GetOwner());
		
		GetGame().GetCallqueue().Remove(StopEffects);
		StartEffects();
		ApplyParamsToEffects();
		GetGame().GetCallqueue().CallLater(StopEffects, m_fEmittingTimeMS);
	}
	
	//------------------------------------------------------------------------------------------------
	void InitEffects(IEntity weapon)
	{
		foreach (ACE_Overheating_SmokeEffect effect : m_aSomkeEffects)
		{
			effect.Init(weapon);
		}
		
		m_bInitDone = true;
	}
	
	//------------------------------------------------------------------------------------------------
	void ApplyParamsToEffects()
	{
		foreach (ACE_Overheating_SmokeEffect effect : m_aSomkeEffects)
		{
			Particles particles = effect.GetEffectEntity().GetParticles();
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
		foreach (ACE_Overheating_SmokeEffect effect : m_aSomkeEffects)
		{
			effect.GetEffectEntity().Play();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void StopEffects()
	{
		foreach (ACE_Overheating_SmokeEffect effect : m_aSomkeEffects)
		{
			effect.GetEffectEntity().StopEmission();
			
			ParticleEffectEntity fadeOut = effect.SpawnFadeOutEffect();
			Particles particles = fadeOut.GetParticles();
			particles.SetParam(-1, EmitterParam.BIRTH_RATE, m_fBirthRate);
			particles.SetParam(-1, EmitterParam.LIFETIME, m_fLifetime);
			particles.SetParam(-1, EmitterParam.LIFETIME_RND, m_pData.GetLifetimeRndScale() * m_fLifetime);
			particles.SetParam(-1, EmitterParam.SIZE, m_fSize);
			particles.SetParam(-1, EmitterParam.VELOCITY, m_fVelocity);
		}
	}
}

//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerResourceTitleField("m_sContinuousParticleEffect")]
class ACE_Overheating_SmokeEffect : Managed
{
	[Attribute(uiwidget: UIWidgets.ResourceNamePicker, params: "ptc")]
	protected ResourceName m_sContinuousParticleEffect;
	
	[Attribute(uiwidget: UIWidgets.ResourceNamePicker, params: "ptc")]
	protected ResourceName m_sFadeoutParticleEffect;
	
	[Attribute()]
	protected ref PointInfo m_pEffectPosition;
	
	protected ref ParticleEffectEntitySpawnParams m_pSpawnParams;
	protected ParticleEffectEntity m_pEffectEntity;
	
	//------------------------------------------------------------------------------------------------
	void Init(IEntity weapon)
	{
		Animation weaponAnim = weapon.GetAnimation();
		m_pSpawnParams = new ParticleEffectEntitySpawnParams();
		m_pSpawnParams.Parent = weapon;
		m_pEffectPosition.Init(weapon);
		m_pSpawnParams.PivotID = m_pEffectPosition.GetNodeId();
		m_pEffectPosition.GetLocalTransform(m_pSpawnParams.Transform);
		m_pSpawnParams.DeleteWhenStopped = false;
		m_pEffectEntity = ParticleEffectEntity.SpawnParticleEffect(m_sContinuousParticleEffect, m_pSpawnParams);
	}
	
	//------------------------------------------------------------------------------------------------
	ParticleEffectEntity GetEffectEntity()
	{
		return m_pEffectEntity;
	}
	
	//------------------------------------------------------------------------------------------------
	ParticleEffectEntity SpawnFadeOutEffect()
	{
		m_pSpawnParams.DeleteWhenStopped = true;
		return ParticleEffectEntity.SpawnParticleEffect(m_sFadeoutParticleEffect, m_pSpawnParams);
	}
	
	//------------------------------------------------------------------------------------------------
	void ~ACE_Overheating_SmokeEffect()
	{
		SCR_EntityHelper.DeleteEntityAndChildren(m_pEffectEntity);
	}
}
