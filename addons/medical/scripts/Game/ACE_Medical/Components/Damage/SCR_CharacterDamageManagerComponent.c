//------------------------------------------------------------------------------------------------
//! Introduce a second chance, which gets triggered when the character would have died without
//! falling unconscious.
//! Add methods for interacting with pain hit zone.
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	[Attribute(defvalue: "600", desc: "Time to regenerate this hit zone fully in second chance state [s]", category: "ACE Medical")]
	protected float m_fACE_Medical_SecondChanceFullRegenetationTimeS;

	protected HitZone m_pACE_Medical_HealthHitZone;
	protected float m_fACE_Medical_CriticalHealth;
	protected ref array<HitZone> m_aACE_Medical_PhysicalHitZones = {};

	protected ACE_Medical_PainHitZone m_pACE_Medical_PainHitZone;
	protected float m_fACE_Medical_ModeratePainThreshold;
	protected float m_fACE_Medical_SeriousPainThreshold;
	
	// We only notify the replication system about changes of these members on initialization
	// After init, each proxy is itself responsible for updating these members
	// Having them as RplProp also ensures that JIPs receive the current state from the server
	[RplProp()]
	protected bool m_bACE_Medical_Initialized = false;
	[RplProp()]
	protected bool m_bACE_Medical_HasSecondChance = false;
	[RplProp()]
	protected bool m_bACE_Medical_SecondChanceOnHeadEnabled = false;
	[RplProp()]
	protected bool m_bACE_Medical_SecondChanceTriggered = false;
	protected float m_fACE_Medical_SecondChanceRegenScale;
	
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
		
		m_fACE_Medical_SecondChanceRegenScale = -GetResilienceHitZone().GetMaxHealth() / m_fACE_Medical_SecondChanceFullRegenetationTimeS;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Initialize ACE medical on a character damage manager (Called on the server)
	void ACE_Medical_Initialize()
	{
		if (m_bACE_Medical_Initialized)
			return;
				
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (settings)
			m_bACE_Medical_SecondChanceOnHeadEnabled = settings.m_bSecondChanceOnHeadEnabled;
		
		ACE_Medical_EnableSecondChance(true);
		// Damage calculations are done on all machines, so we have to broadcast the init
		m_bACE_Medical_Initialized = true;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	// Reduce regeneration rate when second chance was triggered
	override float GetResilienceRegenScale()
	{
		float scale = super.GetResilienceRegenScale();
		if (m_bACE_Medical_SecondChanceTriggered && scale > 0)
		{
			return m_fACE_Medical_SecondChanceRegenScale;
		}
		else
		{
			return scale;
		}
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
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns true if ACE Medical has been initialized
	bool ACE_Medical_IsInitialized()
	{
		return m_bACE_Medical_Initialized;
	}

	//------------------------------------------------------------------------------------------------
	//! Enable/disable second chance
	void ACE_Medical_EnableSecondChance(bool enable)
	{
		m_bACE_Medical_HasSecondChance = enable;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check if second chance is enabled
	bool ACE_Medical_HasSecondChance()
	{
		return m_bACE_Medical_HasSecondChance;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Returns true if second chance is enabled for the given hit zone
	bool ACE_Medical_HasSecondChanceOnHitZone(HitZone hitZone)
	{
		if (!m_bACE_Medical_HasSecondChance)
			return false;
		
		if (m_bACE_Medical_SecondChanceOnHeadEnabled)
			return true;
		
		return (hitZone != m_pHeadHitZone);
	}
	
	//------------------------------------------------------------------------------------------------
	//! To be set true when second chance was used
	void ACE_Medical_SetSecondChanceTrigged(bool isTriggered)
	{
		m_bACE_Medical_SecondChanceTriggered = isTriggered;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check if second chance was used
	bool ACE_Medical_WasSecondChanceTrigged()
	{
		return m_bACE_Medical_SecondChanceTriggered;
	}
}
