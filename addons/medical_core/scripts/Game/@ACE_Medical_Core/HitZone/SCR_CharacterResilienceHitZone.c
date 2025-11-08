//------------------------------------------------------------------------------------------------
modded class SCR_CharacterResilienceHitZone : SCR_RegeneratingHitZone
{
	//------------------------------------------------------------------------------------------------
	//! Multiply regen by the regen scale and all other damage by the damage scale
	override float ComputeEffectiveDamage(notnull BaseDamageContext damageContext, bool isDOT)
	{
		float damage = super.ComputeEffectiveDamage(damageContext, isDOT);

		if (!Replication.IsServer())
			return damage;

		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
		if (!damageManager)
			return damage;

		if (damageContext.damageType == EDamageType.REGENERATION)
			return damage * damageManager.ACE_Medical_GetResilienceRegenScale();

		return damage * damageManager.ACE_Medical_GetResilienceDamageScale();
	}
}
