//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerEvaluator : DamageEffectEvaluator
{
	//------------------------------------------------------------------------------------------------
	override void HandleEffectConsequences(SCR_BaseBulletDamageEffect effect, ExtendedDamageManagerComponent dmgManager)
	{
		super.HandleEffectConsequences(effect, dmgManager);
		ACE_Medical_BreakSplint(effect, dmgManager);
	}
	
	//------------------------------------------------------------------------------------------------
	override void HandleEffectConsequences(SCR_CollisionDamageEffect effect, ExtendedDamageManagerComponent dmgManager)
	{
		super.HandleEffectConsequences(effect, dmgManager);
		ACE_Medical_BreakSplint(effect, dmgManager);
	}
	
	//------------------------------------------------------------------------------------------------
	override void HandleEffectConsequences(SCR_FragmentationDamageEffect effect, ExtendedDamageManagerComponent dmgManager)
	{
		super.HandleEffectConsequences(effect, dmgManager);
		ACE_Medical_BreakSplint(effect, dmgManager);
	}
	
	//------------------------------------------------------------------------------------------------
	override void HandleEffectConsequences(SCR_MeleeDamageEffect effect, ExtendedDamageManagerComponent dmgManager)
	{
		super.HandleEffectConsequences(effect, dmgManager);
		ACE_Medical_BreakSplint(effect, dmgManager);
	}

	//------------------------------------------------------------------------------------------------
	override void HandleEffectConsequences(SCR_CriticalExplosionDamageEffect effect, ExtendedDamageManagerComponent dmgManager)
	{
		super.HandleEffectConsequences(effect, dmgManager);
		ACE_Medical_BreakSplint(effect, dmgManager);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Removes splint damage effect on affected hit zone
	protected void ACE_Medical_BreakSplint(SCR_InstantDamageEffect effect, ExtendedDamageManagerComponent dmgManager)
	{
		SCR_CharacterHitZone affectedHitZone = SCR_CharacterHitZone.Cast(effect.GetAffectedHitZone());
		if (!affectedHitZone)
			return;
		
		ECharacterHitZoneGroup group = affectedHitZone.GetHitZoneGroup();
		if (group != ECharacterHitZoneGroup.LEFTLEG && group != ECharacterHitZoneGroup.LEFTARM && group != ECharacterHitZoneGroup.RIGHTARM && group != ECharacterHitZoneGroup.RIGHTLEG)
			return;
		
		array<ref SCR_PersistentDamageEffect> splintEffects = {};
		dmgManager.FindAllDamageEffectsOfType(ACE_Medical_SplintDamageEffect, splintEffects);
		
		if (splintEffects.IsEmpty())
			return;
		
		array<HitZone> groupHitZones = {};
		dmgManager.GetHitZonesOfGroup(group, groupHitZones);
		
		foreach (SCR_PersistentDamageEffect splintEffect : splintEffects)
		{
			if (groupHitZones.Contains(splintEffect.GetAffectedHitZone()))
				dmgManager.TerminateDamageEffect(splintEffect);
		}
	}
}
