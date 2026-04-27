//------------------------------------------------------------------------------------------------
//! User action used to remove an active splint from a limb.
class ACE_Medical_RemoveSplintUserAction : SCR_MorphineUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		SCR_ChimeraCharacter targetCharacter = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!targetCharacter)
			return false;

		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(targetCharacter.GetDamageManager());
		if (!damageManager)
			return false;

		array<HitZone> groupHitZones = {};
		damageManager.GetHitZonesOfGroup(m_eHitZoneGroup, groupHitZones);

		if (!damageManager.IsDamageEffectPresentOnHitZones(ACE_Medical_SplintDamageEffect, groupHitZones))
			return false;

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_ChimeraCharacter targetCharacter = SCR_ChimeraCharacter.Cast(pOwnerEntity);
		if (!targetCharacter)
			return;

		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(targetCharacter.GetDamageManager());
		if (!damageManager)
			return;

		damageManager.TerminateDamageEffectsOfType(ACE_Medical_SplintDamageEffect);
	}
}