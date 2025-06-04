//------------------------------------------------------------------------------------------------
modded class SCR_CharacterResilienceHitZone : SCR_RegeneratingHitZone
{
	protected SCR_CharacterDamageManagerComponent m_pACE_Medical_DamageManager;
	
	//-----------------------------------------------------------------------------------------------------------
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		m_pACE_Medical_DamageManager = SCR_CharacterDamageManagerComponent.Cast(pManagerComponent);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Multiply regen by the regen scale
	override float ComputeEffectiveDamage(notnull BaseDamageContext damageContext, bool isDOT)
	{
		float damage = super.ComputeEffectiveDamage(damageContext, isDOT);
		
		if (Replication.IsServer() && damageContext.damageType == EDamageType.REGENERATION)
			damage *= m_pACE_Medical_DamageManager.ACE_Medical_GetResilienceRegenScale();

		return damage;
	}
}
