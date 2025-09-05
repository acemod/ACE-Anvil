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
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Birth rate RND vs temperature [K]", params: "1300 100 0 0")]
	protected ref Curve m_cBirthRateRndTemperatureCurve;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle lifetime multiplier vs temperature [K]", params: "1300 4 0 0")]
	protected ref Curve m_cLifetimeMultiplierMultiplierTemperatureCurve;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle lifetime RND vs temperature [K]", params: "1300 4 0 0")]
	protected ref Curve m_cLifetimeRndMultiplierTemperatureCurve;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle size multiplier vs temperature [K]", params: "1300 1 0 0")]
	protected ref Curve m_cSizeMultiplierTemperatureCurve;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle size RND vs temperature [K]", params: "1300 1 0 0")]
	protected ref Curve m_cSizeRndTemperatureCurve;

	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle velocity vs temperature [K]", params: "1300 3 0 0")]
	protected ref Curve m_cVelocityMultiplierTemperatureCurve;
	
	[Attribute(uiwidget: UIWidgets.GraphDialog, desc: "Particle velocity RND vs temperature [K]", params: "1300 3 0 0")]
	protected ref Curve m_cVelocityRndTemperatureCurve;
	
	protected ACE_Overheating_BarrelComponent m_pBarrel;
	protected static ACE_Overheating_Settings s_pSettings;
		
	//------------------------------------------------------------------------------------------------
	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);
		m_pBarrel = ACE_Overheating_BarrelComponent.FromWeapon(owner);
		
		if (!s_pSettings)
			s_pSettings = ACE_SettingsHelperT<ACE_Overheating_Settings>.GetModSettings();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnFired(IEntity effectEntity, BaseMuzzleComponent muzzle, IEntity projectileEntity)
	{
		super.OnFired(effectEntity, muzzle, projectileEntity);
		
		if (!m_pBarrel || m_pBarrel.GetBarrelTemperature() < s_pSettings.m_fMinSmokeTemperature)
			return;
		
		IEntity weapon = muzzle.GetOwner();
		Animation weaponAnim = weapon.GetAnimation();
		ParticleEffectEntitySpawnParams spawnParams = new ParticleEffectEntitySpawnParams();
		spawnParams.Parent = weapon;
		spawnParams.PivotID = weaponAnim.GetBoneIndex("barrel_muzzle");
		ParticleEffectEntity muzzleEffect = ParticleEffectEntity.SpawnParticleEffect(m_sMuzzleSmokeEffectName, spawnParams);
		
		//spawnParams.Transform[3] - 
		ParticleEffectEntity barrelEffect = ParticleEffectEntity.SpawnParticleEffect(m_sBarrelSurfaceSmokeEffectName, spawnParams);
	}
}
