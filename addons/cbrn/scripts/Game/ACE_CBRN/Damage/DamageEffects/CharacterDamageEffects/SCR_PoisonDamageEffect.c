//------------------------------------------------------------------------------------------------
modded class SCR_PoisonDamageEffect : SCR_DotDamageEffect
{
	//------------------------------------------------------------------------------------------------
	override void OnEffectAdded(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		super.OnEffectAdded(dmgManager);
		
		SCR_CharacterDamageManagerComponent characterDamageMgr = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
		if (characterDamageMgr)
			characterDamageMgr.ACE_Medical_UpdateResilienceRegenScale();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Add regen if target hit zone is virtual
	override void OnEffectRemoved(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		super.OnEffectRemoved(dmgManager);
		
		SCR_CharacterDamageManagerComponent characterDamageMgr = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
		if (!characterDamageMgr)
			return;
		
		characterDamageMgr.ACE_Medical_UpdateResilienceRegenScale();
		
		SCR_RegeneratingHitZone affectedHitzone = SCR_RegeneratingHitZone.Cast(GetAffectedHitZone());
		if (affectedHitzone && affectedHitzone.GetHitZoneGroup() == ECharacterHitZoneGroup.VIRTUAL)
			characterDamageMgr.RegenVirtualHitZone(affectedHitzone);
	}
}
