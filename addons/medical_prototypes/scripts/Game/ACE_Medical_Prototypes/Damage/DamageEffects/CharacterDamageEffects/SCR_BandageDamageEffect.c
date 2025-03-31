//------------------------------------------------------------------------------------------------
modded class SCR_BandageDamageEffect: SCR_InstantDamageEffect
{
	//------------------------------------------------------------------------------------------------
	//! Only stop strongest bleeding rather than all on the hit zone group
	override void OnEffectAdded(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		if (!dmgManager)
			return;
		
		SCR_CharacterHitZone charHitZone = SCR_CharacterHitZone.Cast(GetAffectedHitZone());
		if (!charHitZone)
			return;
		
		EHitZoneGroup hitZoneGroup = charHitZone.GetHitZoneGroup();
		SCR_BleedingDamageEffect strongestBleedingEffect;
		float largestBleedingRate = 0;
		
		array<ref SCR_PersistentDamageEffect> effects = {};
		dmgManager.FindAllDamageEffectsOfType(SCR_BleedingDamageEffect, effects);
		
		foreach (SCR_PersistentDamageEffect effect : effects)
		{
			SCR_BleedingDamageEffect bleedingEffect = SCR_BleedingDamageEffect.Cast(effect);
			if (!bleedingEffect)
				continue;
			
			charHitZone = SCR_CharacterHitZone.Cast(effect.GetAffectedHitZone());
			if (!charHitZone)
				continue;
			
			if (charHitZone.GetHitZoneGroup() != hitZoneGroup)
				continue;
			
			float bleedingRate = bleedingEffect.CalculateBleedingRate();
			if (bleedingRate > largestBleedingRate)
			{
				largestBleedingRate = bleedingRate;
				strongestBleedingEffect = bleedingEffect;
			}
		}
		
		if (strongestBleedingEffect)
			dmgManager.TerminateDamageEffect(strongestBleedingEffect);
	}
}
