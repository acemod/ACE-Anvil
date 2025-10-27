//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	protected ACE_Medical_PainHitZone m_pACE_Medical_PainHitZone;
	
	[RplProp(condition: RplCondition.OwnerOnly)]
	protected float m_fACE_Medical_PainSuppression = 0;
	
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
		if (!m_pACE_Medical_PainHitZone)
			return 0;
		
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
}
