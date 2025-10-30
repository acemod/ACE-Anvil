//------------------------------------------------------------------------------------------------
class ACE_Overheating_SmokeEffectComponentClass : MuzzleEffectComponentClass
{
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Birth rate vs temperature [K]", params: "1300 100 0 0")]
	protected ref Curve m_cBirthRateTemperatureCurve;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle lifetime [s] vs temperature [K]", params: "1300 4 0 0")]
	protected ref Curve m_cLifetimeTemperatureCurve;
	
	[Attribute(defvalue: "2", "Precision of life time [s]", params: "0 8")]
	protected int m_iLifetimePrecision;
	
	[Attribute(defvalue: "1", desc: "Particle lifetime Rnd scale", params: "0 10")]
	protected float m_fLifetimeRndScale;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle size vs temperature [K]", params: "1300 1 0 0")]
	protected ref Curve m_cSizeTemperatureCurve;
	
	[Attribute(defvalue: "2", "Precision of size", params: "0 8")]
	protected int m_iSizePrecision;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle velocity [m/s] vs temperature [K]", params: "1300 3 0 0")]
	protected ref Curve m_cVelocityTemperatureCurve;
	
	[Attribute(defvalue: "2", "Precision of velocity", params: "0 8")]
	protected int m_iVelocityPrecision;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle emitting time [s] vs temperature [K]", params: "1300 60 0 0")]
	protected ref Curve m_cEmittingTimeTemperatureCurve;
	
	[Attribute(defvalue: "2", "Precision of velocity", params: "0 8")]
	protected int m_iEmittingTimePrecision;
	
	//------------------------------------------------------------------------------------------------
	float GetLifetimeRndScale()
	{
		return m_fLifetimeRndScale;
	}
	
	//------------------------------------------------------------------------------------------------
	int ComputeBirthRate(float temperature)
	{
		return Math.Round(Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cBirthRateTemperatureCurve)[1]);
	}
	
	//------------------------------------------------------------------------------------------------
	float ComputeLifetime(float temperature)
	{
		return ACE_Math.Round(Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cLifetimeTemperatureCurve)[1], m_iLifetimePrecision);
	}
	
	//------------------------------------------------------------------------------------------------
	float ComputeSize(float temperature)
	{
		return ACE_Math.Round(Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cSizeTemperatureCurve)[1], m_iSizePrecision);
	}
	
	//------------------------------------------------------------------------------------------------
	float ComputeVelocity(float temperature)
	{
		return ACE_Math.Round(Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cVelocityTemperatureCurve)[1], m_iVelocityPrecision);
	}
	
	//------------------------------------------------------------------------------------------------
	float ComputeEmittingTime(float temperature)
	{
		return ACE_Math.Round(Math3D.Curve(ECurveType.CurveProperty2D, temperature, m_cEmittingTimeTemperatureCurve)[1], m_iEmittingTimePrecision);
	}
}

//------------------------------------------------------------------------------------------------
class ACE_Overheating_SmokeEffectComponent : MuzzleEffectComponent
{
	[Attribute()]
	protected ref array<ref ACE_Overheating_SmokeEffectConfig> m_aSomkeEffects;
	
	[RplProp()]
	protected bool m_bSmokeEnabled = false;
	[RplProp()]
	protected int m_iBirthRate;
	[RplProp()]
	protected float m_fLifetime;
	[RplProp()]
	protected float m_fSize;
	[RplProp()]
	protected float m_fVelocity;
	[RplProp()]
	protected float m_fEmittingTimeMS;
	
	protected ACE_Overheating_SmokeEffectComponentClass m_pData;
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
		if (!s_pSettings)
			return;
		
		m_bSmokeEnabled = (m_pBarrel.GetBarrelTemperature() >= s_pSettings.m_fMinSmokeTemperature);
		if (m_bSmokeEnabled)
		{
			m_iBirthRate = m_pData.ComputeBirthRate(temperature);
			m_fLifetime = m_pData.ComputeLifetime(temperature);
			m_fSize = m_pData.ComputeSize(temperature);
			m_fVelocity = m_pData.ComputeVelocity(temperature);
			m_fEmittingTimeMS = 1000 * m_pData.ComputeEmittingTime(temperature);
		}
		
		Replication.BumpMe();
		
	#ifdef ENABLE_DIAG
		PrintFormat("ACE_Overheating_SmokeEffectComponent::UpdateParams");
		Print("----------");
		Print(m_iBirthRate);
		Print(m_fLifetime);
		Print(m_fSize);
		Print(m_fVelocity);
		Print(m_fEmittingTimeMS);
		Print("----------");
	#endif
	}
		
	//------------------------------------------------------------------------------------------------
	override void OnFired(IEntity effectEntity, BaseMuzzleComponent muzzle, IEntity projectileEntity)
	{
		super.OnFired(effectEntity, muzzle, projectileEntity);
		
		if (!m_bSmokeEnabled)
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
		foreach (ACE_Overheating_SmokeEffectConfig effectConfig : m_aSomkeEffects)
		{
			effectConfig.Init(weapon);
		}
		
		m_bInitDone = true;
	}
	
	//------------------------------------------------------------------------------------------------
	void ApplyParamsToEffects()
	{
		foreach (ACE_Overheating_SmokeEffectConfig effectConfig : m_aSomkeEffects)
		{
			ApplyParamsToEffectEntity(effectConfig.GetEffectEntity());
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ApplyParamsToEffectEntity(ParticleEffectEntity effectEntity)
	{
		Particles particles = effectEntity.GetParticles();
		particles.SetParam(-1, EmitterParam.BIRTH_RATE, m_iBirthRate);
		particles.SetParam(-1, EmitterParam.LIFETIME, m_fLifetime);
		particles.SetParam(-1, EmitterParam.LIFETIME_RND, m_pData.GetLifetimeRndScale() * m_fLifetime);
		particles.SetParam(-1, EmitterParam.SIZE, m_fSize);
		particles.SetParam(-1, EmitterParam.VELOCITY, m_fVelocity);
	}
	
	//------------------------------------------------------------------------------------------------
	void StartEffects()
	{
	#ifdef ENABLE_DIAG
		Print("ACE_Overheating_SmokeEffectComponent::StartEffects");
	#endif
		
		foreach (ACE_Overheating_SmokeEffectConfig effectConfig : m_aSomkeEffects)
		{
			effectConfig.GetEffectEntity().Play();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Stops effect with a fade-out
	void StopEffects()
	{
	#ifdef ENABLE_DIAG
		Print("ACE_Overheating_SmokeEffectComponent::StopEffects");
	#endif
		
		foreach (ACE_Overheating_SmokeEffectConfig effectConfig : m_aSomkeEffects)
		{
			effectConfig.GetEffectEntity().StopEmission();
			ApplyParamsToEffectEntity(effectConfig.SpawnFadeOutEffect());
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Instantly terminates effects
	void TerminateEffects()
	{
		if (!System.IsConsoleApp())
			RpcDo_TerminateEffectsBroadcast();
		
		Rpc(RpcDo_TerminateEffectsBroadcast);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_TerminateEffectsBroadcast()
	{
		GetGame().GetCallqueue().Remove(StopEffects);
		
		foreach (ACE_Overheating_SmokeEffectConfig effectConfig : m_aSomkeEffects)
		{
			effectConfig.GetEffectEntity().Stop();
		}
	}
}

//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseContainerResourceTitleField("m_sContinuousParticleEffect")]
class ACE_Overheating_SmokeEffectConfig : Managed
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
	void ~ACE_Overheating_SmokeEffectConfig()
	{
		SCR_EntityHelper.DeleteEntityAndChildren(m_pEffectEntity);
	}
}
