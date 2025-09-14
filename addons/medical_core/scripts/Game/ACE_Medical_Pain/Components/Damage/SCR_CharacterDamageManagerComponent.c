//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	protected ACE_Medical_PainHitZone m_pACE_Medical_PainHitZone;

	[RplProp(condition: RplCondition.OwnerOnly)]
	protected float m_fACE_Medical_PainSuppression = 0;

	[Attribute(defvalue: "0.25", uiwidget: UIWidgets.Slider, desc: "Percentage of armor damage that gets converted to pain", params: "0 1 0.01",)]
	protected float m_fACE_Medical_ArmorPainMultiplier;

	[Attribute(defvalue: "10", uiwidget: UIWidgets.Slider, desc: "Minimum armor damage required to cause pain", params: "0 50 1")]
	protected float m_fACE_Medical_ArmorPainThreshold;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Called by ACE_Medical_PainHitZone.OnInit to initialize the hit zone
	void ACE_Medical_SetPainHitZone(HitZone hitzone)
	{
		m_pACE_Medical_PainHitZone = ACE_Medical_PainHitZone.Cast(hitzone);
	}

	//-----------------------------------------------------------------------------------------------------------
	//! Return the pain hit zone
	ACE_Medical_PainHitZone ACE_Medical_GetPainHitZone()
	{
		return m_pACE_Medical_PainHitZone;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	bool ACE_Medical_IsInPain()
	{
		return m_pACE_Medical_PainHitZone.GetHealthScaled() + m_fACE_Medical_PainSuppression <= m_pACE_Medical_PainHitZone.GetDamageStateThreshold(ECharacterHealthState.MODERATE);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Returns intensity of pain used for ACE_Medical_PainScreenEffect
	//! - 0 when less than moderate damage
	//! - Linear response from full health -> 0 to serious damage -> 1
	float ACE_Medical_GetPainIntensity()
	{
		// Account for analgesic effects
		float health = m_pACE_Medical_PainHitZone.GetHealthScaled() + m_fACE_Medical_PainSuppression;
		
		if (health > m_pACE_Medical_PainHitZone.GetDamageStateThreshold(ECharacterHealthState.MODERATE))
			return 0;
		
		if (health <= m_pACE_Medical_PainHitZone.GetDamageStateThreshold(ECharacterHealthState.SERIOUS))
			return 1;
		
		return Math.InverseLerp(1, m_pACE_Medical_PainHitZone.GetDamageStateThreshold(ECharacterHealthState.SERIOUS), health);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	void ACE_Medical_SetPainSuppression(float percentage)
	{
		m_fACE_Medical_PainSuppression = percentage;
		Replication.BumpMe();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	float ACE_Medical_GetPainSuppression()
	{
		return m_fACE_Medical_PainSuppression;
	}

	//------------------------------------------------------------------------------------------------
	//! Add pain when armor blocks damage
	//! Behavior:
	//! - Small impacts below threshold are ignored
	//! - First significant impact sets character to moderate pain state
	//! - Subsequent hits while already in pain can worsen the condition based on the multiplier
	override void ArmorHitEventDamage(EDamageType type, float damage, IEntity instigator)
	{
		super.ArmorHitEventDamage(type, damage, instigator);

		if (!m_pACE_Medical_PainHitZone)
			return;

		if (damage < m_fACE_Medical_ArmorPainThreshold)
			return;

		if (!ACE_Medical_IsInPain())
		{
			float painDamage = damage * m_fACE_Medical_ArmorPainMultiplier;

			float moderateThreshold = m_pACE_Medical_PainHitZone.GetDamageStateThreshold(ECharacterHealthState.MODERATE);
			float currentHealth = m_pACE_Medical_PainHitZone.GetHealthScaled();
			float minDamageForPain = (currentHealth - moderateThreshold) * m_pACE_Medical_PainHitZone.GetMaxHealth();

			painDamage = Math.Max(painDamage, minDamageForPain);
			m_pACE_Medical_PainHitZone.HandleDamage(painDamage, EDamageType.TRUE, instigator);
		}
		else
		{
			// Already in pain - add more based on multiplier
			float painDamage = damage * m_fACE_Medical_ArmorPainMultiplier;
			if (painDamage > 0)
			{
				m_pACE_Medical_PainHitZone.HandleDamage(painDamage, EDamageType.TRUE, instigator);
			}
		}
	}
}
