//-----------------------------------------------------------------------------------------------------------
//! health hitzone - Receives damage from physical hitzones
modded class SCR_CharacterHealthHitZone : SCR_HitZone
{
	protected SCR_CharacterDamageManagerComponent m_pACE_Medical_DamageManager;
	protected bool m_bACE_Medical_HasSecondChanceOnHead;
	protected float ACE_MEDICAL_SECOND_CHANCE_MIN_HEALTH = 1;
	
	//-----------------------------------------------------------------------------------------------------------
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		m_pACE_Medical_DamageManager = SCR_CharacterDamageManagerComponent.Cast(pManagerComponent);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Change damage calculation while second chance is enabled
	override float ComputeEffectiveDamage(notnull BaseDamageContext damageContext, bool isDOT)
	{
		if (m_pACE_Medical_DamageManager.ACE_Medical_HasSecondChanceOnHitZone(damageContext.struckHitZone))
			return ACE_Medical_ComputeSecondChanceEffectiveDamage(damageContext, isDOT);
		
		return super.ComputeEffectiveDamage(damageContext, isDOT);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Computes the effective damage while the character has second chance enabled
	//! Ensures that the hit zone health can never drop below ACE_MEDICAL_SECOND_CHANCE_MIN_HEALTH
	//! Triggers second chance if damage had to be rescaled
	protected float ACE_Medical_ComputeSecondChanceEffectiveDamage(notnull BaseDamageContext damageContext, bool isDOT)
	{
		float effectiveDamage = super.ComputeEffectiveDamage(damageContext, isDOT);
		if (GetHealth() - effectiveDamage >= ACE_MEDICAL_SECOND_CHANCE_MIN_HEALTH)
			return effectiveDamage;
		
		SCR_CharacterHitZone hitZone = SCR_CharacterHitZone.Cast(damageContext.struckHitZone);
		if (hitZone && !m_pACE_Medical_DamageManager.ACE_Medical_WasSecondChanceTrigged())
		{
			m_pACE_Medical_DamageManager.ACE_Medical_SetSecondChanceTrigged(true);
			// Add bleeding to the hit zone that triggered second chance
			hitZone.AddBleeding(damageContext.colliderID);
		}
		
		return GetHealth() - ACE_MEDICAL_SECOND_CHANCE_MIN_HEALTH;
	}
}
