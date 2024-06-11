//------------------------------------------------------------------------------------------------
//! Introduce a second chance, which gets triggered when the character would have died without
//! falling unconscious.
//! Add methods for interacting with pain hit zone.
modded class SCR_CharacterDamageManagerComponent
{
	private static const float ACE_MEDICAL_SECOND_CHANCE_DEACTIVATION_TIMEOUT_MS = 1000;

	protected HitZone m_pACE_Medical_HealthHitZone;
	protected float m_fACE_Medical_CriticalHealth;

	protected ACE_Medical_PainHitZone m_pACE_Medical_PainHitZone;
	protected float m_fACE_Medical_ModeratePainThreshold;
	protected float m_fACE_Medical_SeriousPainThreshold;

	[RplProp()]
	protected float m_fACE_Medical_SecondChanceRegenScale;
	[RplProp()]
	protected bool m_bACE_Medical_HasSecondChance = false;
	[RplProp()]
	protected bool m_bACE_Medical_SecondChanceOnHeadEnabled = false;
	[RplProp()]
	protected bool m_bACE_Medical_SecondChanceTriggered = false;
	
	private RplComponent m_RplComponent;
	
	
	//-----------------------------------------------------------------------------------------------------------
	//! Initialize member variables
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		m_RplComponent = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		
		m_pACE_Medical_HealthHitZone = GetHitZoneByName("Health");
		if (!m_pACE_Medical_HealthHitZone)
			return;
		
		m_fACE_Medical_CriticalHealth = m_pACE_Medical_HealthHitZone.GetDamageStateThreshold(ECharacterHealthState.CRITICAL);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Initialize ACE medical on a character damage manager (Called on the server)
	void ACE_Medical_Initialize()
	{
		if (ACE_Medical_IsProxy())
			return;
		
		const ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (settings)
		{
			m_bACE_Medical_SecondChanceOnHeadEnabled = settings.m_bSecondChanceOnHeadEnabled;
			m_fACE_Medical_SecondChanceRegenScale = settings.m_fSecondChanceRegenScale;
			Replication.BumpMe();
		}
		
		ACE_Medical_EnableSecondChance(true);
	}
	
	//------------------------------------------------------------------------------------------------
	// Reduce regeneration rate when second chance was triggered
	override float GetResilienceRegenScale()
	{
		const float scale = super.GetResilienceRegenScale();
		if (m_bACE_Medical_SecondChanceTriggered && scale > 0)
			return m_fACE_Medical_SecondChanceRegenScale;
		else
			return scale;
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
		array<HitZone> physicalHitZones = {};
		GetPhysicalHitZones(physicalHitZones);
		
		foreach (HitZone hitZone : physicalHitZones)
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
		const float scaledHealth = Math.Clamp(m_pACE_Medical_PainHitZone.GetHealthScaled(), m_fACE_Medical_SeriousPainThreshold, 1);
		
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
	//! Enable/disable second chance
	void ACE_Medical_EnableSecondChance(bool enable)
	{
		m_bACE_Medical_HasSecondChance = enable;
		Replication.BumpMe();
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
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check if second chance was used
	bool ACE_Medical_WasSecondChanceTrigged()
	{
		return m_bACE_Medical_SecondChanceTriggered;
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		super.OnLifeStateChanged(previousLifeState, newLifeState);

		if (ACE_Medical_IsProxy())
			return;
		
		if (previousLifeState == newLifeState)
			return;
		
		switch (newLifeState)
		{
			// Add second chance when revived
			case ECharacterLifeState.ALIVE:
			{
				GetGame().GetCallqueue().Remove(ACE_Medical_EnableSecondChance);
				ACE_Medical_EnableSecondChance(true);
				ACE_Medical_SetSecondChanceTrigged(false);
				break;
			}
				
			// Schedule removal of second chance when falling unconscious
			case ECharacterLifeState.INCAPACITATED:
			{
				GetGame().GetCallqueue().Remove(ACE_Medical_EnableSecondChance);
				GetGame().GetCallqueue().CallLater(ACE_Medical_EnableSecondChance, ACE_MEDICAL_SECOND_CHANCE_DEACTIVATION_TIMEOUT_MS, false, false);
				break;
			}
				
			// Remove second chance when dead
			case ECharacterLifeState.DEAD:
			{
				GetGame().GetCallqueue().Remove(ACE_Medical_EnableSecondChance);
				ACE_Medical_EnableSecondChance(false);
				break;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	private bool ACE_Medical_IsProxy()
	{
		return m_RplComponent && m_RplComponent.IsProxy();
	}
	
	//------------------------------------------------------------------------------------------------
	void ~SCR_CharacterDamageManagerComponent()
	{
		GetGame().GetCallqueue().Remove(ACE_Medical_EnableSecondChance);
	}
}
