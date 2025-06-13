//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponentClass : SCR_DamageManagerComponentClass
{
	[Attribute(defvalue: "500", desc: "Particle birth rate for massive bleedings")]
	protected int m_iACE_Medical_MassiveBleedingParticleBirthRate;
	
	[Attribute(defvalue: "1", desc: "Particle velocity for massive bleedings")]
	protected float m_fACE_Medical_MassiveBleedingParticleVelocity;
	
	//------------------------------------------------------------------------------------------------
	//! Reconfigures particles params for massive bleeding
	void ACE_Medical_UpgradeToMassiveBleeding(Particles particles)
	{
		particles.SetParam(-1, EmitterParam.BIRTH_RATE, m_iACE_Medical_MassiveBleedingParticleBirthRate);
		particles.SetParam(-1, EmitterParam.VELOCITY, m_fACE_Medical_MassiveBleedingParticleVelocity);
	}
}

//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	//-----------------------------------------------------------------------------------------------------------
	//! Initialize organ hit zones
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode() || !Replication.IsServer())
			return;
		
		if (!m_pACE_Medical_Settings || !m_pACE_Medical_Settings.m_bOrganHitZonesEnabled)
			return;
		
		array<HitZone> hitZones = {};
		GetPhysicalHitZones(hitZones);
		foreach (HitZone hitZone : hitZones)
		{
			SCR_CharacterHitZone charHitZone = SCR_CharacterHitZone.Cast(hitZone);
			if (charHitZone)
				charHitZone.ACE_Medical_InitOrganHitZones(this);
		}
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Handle massive bleeding effects
	override void CreateBleedingParticleEffect(notnull HitZone hitZone, int colliderDescriptorIndex)
	{
		super.CreateBleedingParticleEffect(hitZone, colliderDescriptorIndex);
		
		if (ACE_Medical_NeckHitZone.Cast(hitZone) && m_mBleedingParticles && m_mBleedingParticles.Contains(hitZone))
		{
			SCR_CharacterDamageManagerComponentClass componentData = SCR_CharacterDamageManagerComponentClass.Cast(GetComponentData(GetOwner()));
			if (componentData)
				componentData.ACE_Medical_UpgradeToMassiveBleeding(m_mBleedingParticles[hitZone].GetParticles());
		}
	}
	
#ifdef WORKBENCH
	[Attribute(defvalue: "false", desc: "Should draw debug shapes for hit zones")]
	protected bool m_bACE_Medical_DrawHitZoneDebug;
	
	//-----------------------------------------------------------------------------------------------------------
	override void OnDiag(IEntity owner, float timeSlice)
	{
		if (!m_bACE_Medical_DrawHitZoneDebug)
			return;
		
		array<HitZone> hitZones = {};
		GetAllHitZones(hitZones);
		foreach (HitZone hitZone : hitZones)
		{
			SCR_HitZone scriptedHitZone = SCR_HitZone.Cast(hitZone);
			if (scriptedHitZone)
				scriptedHitZone.DrawDebug();
		}
	}
#endif
}
