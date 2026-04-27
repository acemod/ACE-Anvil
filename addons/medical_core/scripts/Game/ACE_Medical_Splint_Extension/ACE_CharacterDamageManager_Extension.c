//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent
{
	//------------------------------------------------------------------------------------------------
	//! Returns true if the given hitzone group has an active splint effect.
	bool ACE_Medical_IsGroupSplinted(ECharacterHitZoneGroup hitZoneGroup)
	{
		array<HitZone> groupHitZones = {};
		GetHitZonesOfGroup(hitZoneGroup, groupHitZones);

		return IsDamageEffectPresentOnHitZones(ACE_Medical_SplintDamageEffect, groupHitZones);
	}
}