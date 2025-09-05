//------------------------------------------------------------------------------------------------
class ACE_Overheating_SmokeEffectComponentClass : MuzzleEffectComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_Overheating_SmokeEffectComponent : MuzzleEffectComponent
{
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
		ParticleEffectEntity effect = ParticleEffectEntity.SpawnParticleEffect("{5EF5A6DBC1EA1B1D}Particles/Weapon/ACE_Overheating_MuzzleSmoke.ptc", spawnParams);
	}
}
