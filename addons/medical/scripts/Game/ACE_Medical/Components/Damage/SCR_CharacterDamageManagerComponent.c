//------------------------------------------------------------------------------------------------
//! Add methods for interacting with pain hit zone
modded class SCR_CharacterDamageManagerComponent : ScriptedDamageManagerComponent
{
	protected ACE_Medical_PainHitZone m_pACE_Medical_PainHitZone;
	protected float m_fACE_Medical_ModeratePainThreshold;
	protected float m_fACE_Medical_SeriousPainThreshold;

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