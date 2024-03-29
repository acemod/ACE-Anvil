//------------------------------------------------------------------------------------------------
//! Add methods for interacting with pain hit zone
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	protected HitZone m_pACE_Medical_HealthHitZone;
	protected float m_fACE_Medical_CriticalHealth;
	protected ref array<HitZone> m_aACE_Medical_PhysicalHitZones = {};

	protected ACE_Medical_PainHitZone m_pACE_Medical_PainHitZone;
	protected float m_fACE_Medical_ModeratePainThreshold;
	protected float m_fACE_Medical_SeriousPainThreshold;

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

	//-----------------------------------------------------------------------------------------------------------
	//! Called by ACE_Medical_PainHitZone.OnInit to initialize the hit zone
	void ACE_Medical_SetPainHitZone(HitZone hitzone)
	{
		m_pACE_Medical_PainHitZone = ACE_Medical_PainHitZone.Cast(hitzone);
		m_fACE_Medical_ModeratePainThreshold = m_pACE_Medical_PainHitZone.GetDamageStateThreshold(ECharacterHealthState.MODERATE);
		m_fACE_Medical_SeriousPainThreshold = m_pACE_Medical_PainHitZone.GetDamageStateThreshold(ECharacterHealthState.SERIOUS);
	}

	//-----------------------------------------------------------------------------------------------------------
	//! Return the pain hit zone
	ACE_Medical_PainHitZone ACE_Medical_GetPainHitZone()
	{
		return m_pACE_Medical_PainHitZone;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Returns true if pain hit zone is at least moderately damaged
	bool ACE_Medical_IsInPain()
	{
		return m_pACE_Medical_PainHitZone.GetHealthScaled() <= m_fACE_Medical_ModeratePainThreshold;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Returns intensity of pain used for ACE_Medical_PainScreenEffect
	float ACE_Medical_GetPainIntensity()
	{
		// Clamp between serious damage and full health
		float scaledHealth = Math.Clamp(m_pACE_Medical_PainHitZone.GetHealthScaled(), m_fACE_Medical_SeriousPainThreshold, 1);
		
		if (scaledHealth > m_fACE_Medical_ModeratePainThreshold)
		{
			// No effect when less than moderate damage
			return 0;
		}
		else
		{
			// Linear response from full health -> 0 to serious damage -> 1
			return Math.InverseLerp(1, m_fACE_Medical_SeriousPainThreshold, scaledHealth);
		};
	}
}
