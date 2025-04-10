//-----------------------------------------------------------------------------------------------------------
//! Blood - does not receive damage directly, only via scripted events.
modded class SCR_CharacterBloodHitZone : SCR_RegeneratingHitZone
{
	protected SCR_CharacterDamageManagerComponent m_pACE_Medical_DamageManager;
	
	protected float m_fACE_Medical_LastHealth;
	protected float m_fACE_Medical_LastTimeMS;
	protected float m_fACE_Medical_CurrentHealth;
	protected float m_fACE_Medical_CurrentTimeMS;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Initialize member variables
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		m_pACE_Medical_DamageManager = SCR_CharacterDamageManagerComponent.Cast(pManagerComponent);
		m_fACE_Medical_LastHealth = GetMaxHealth();
		m_fACE_Medical_CurrentHealth = GetMaxHealth();
		m_fACE_Medical_LastTimeMS = GetGame().GetWorld().GetWorldTime();
		m_fACE_Medical_CurrentTimeMS = m_fACE_Medical_LastTimeMS;
	}

	//-----------------------------------------------------------------------------------------------------------
	//! Ensure that the maximum total bleeding rate given by the cardiac output cannot be exceeded
	//! TO DO: Find a better way to do this via a damage effect event-based approach
	override float ComputeEffectiveDamage(notnull BaseDamageContext damageContext, bool isDOT)
	{
		float damage = super.ComputeEffectiveDamage(damageContext, isDOT);
		// TO DO: Directly apply this scale to bleeding and saline damage effects?
		// TO DO: Ensure that saline still delivers same total volume
		damage *= m_pACE_Medical_DamageManager.ACE_Medical_GetBloodFlowScale();
		
		if (damage < 0 || !isDOT)
			return damage;
		
		if (GetGame().GetWorld().GetWorldTime() > m_fACE_Medical_CurrentTimeMS)
		{
			m_fACE_Medical_LastTimeMS = m_fACE_Medical_CurrentTimeMS;
			m_fACE_Medical_CurrentTimeMS = GetGame().GetWorld().GetWorldTime();
			m_fACE_Medical_LastHealth = m_fACE_Medical_CurrentHealth;
		}
		
		m_fACE_Medical_CurrentHealth = GetHealth();
		float timeSlice = (m_fACE_Medical_CurrentTimeMS- m_fACE_Medical_LastTimeMS) / 1000;
		float maxDamage = m_pACE_Medical_DamageManager.ACE_Medical_GetMaxTotalBleedingRate() * timeSlice;
		// Subtract already lost health
		maxDamage -= m_fACE_Medical_LastHealth - m_fACE_Medical_CurrentHealth;
		// Math.Clamp doesn't work here as maxDamage can be negative
		return Math.Max(0, Math.Min(damage, maxDamage));
	}
}
