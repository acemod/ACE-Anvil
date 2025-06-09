//------------------------------------------------------------------------------------------------
modded class SCR_CharacterResilienceHitZone : SCR_RegeneratingHitZone
{
	//------------------------------------------------------------------------------------------------
	//! Multiply regen by the regen scale
	override float ComputeEffectiveDamage(notnull BaseDamageContext damageContext, bool isDOT)
	{
		float damage = super.ComputeEffectiveDamage(damageContext, isDOT);
		
		if (!Replication.IsServer() || damageContext.damageType != EDamageType.REGENERATION)
			return damage;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
		if (!damageManager)
			return damage;
		
		return damage * damageManager.ACE_Medical_GetResilienceRegenScale();
	}
}
