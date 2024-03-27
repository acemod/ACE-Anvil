//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	protected HitZone m_pACE_Medical_HealthHitZone;
	protected float m_fACE_Medical_CriticalHealth;
	protected ref array<HitZone> m_aACE_Medical_PhysicalHitZones = {};
		
	//-----------------------------------------------------------------------------------------------------------
	//! Initialize member variables
	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);
		
		m_pACE_Medical_HealthHitZone = GetHitZoneByName("Health");
		if (!m_pACE_Medical_HealthHitZone)
			return;
		
		m_fACE_Medical_CriticalHealth = m_pACE_Medical_HealthHitZone.GetDamageStateThreshold(ECharacterHealthState.CRITICAL);
		GetPhysicalHitZones(m_aACE_Medical_PhysicalHitZones);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns true if health hit zone has critical health
	bool ACE_Medical_HasCriticalHealth()
	{
		return m_pACE_Medical_HealthHitZone.GetHealthScaled() <= m_fACE_Medical_CriticalHealth;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns true if at least one physical hit zone is injured
	bool ACE_Medical_CanBeHealed()
	{
		foreach (HitZone hitZone : m_aACE_Medical_PhysicalHitZones)
		{
			if (hitZone.GetHealthScaled() < 0.999)
				return true;
		}
		
		return false;
	}
}