//------------------------------------------------------------------------------------------------
modded class SCR_BleedingDamageEffect : SCR_DotDamageEffect
{
	//------------------------------------------------------------------------------------------------
	//! Same as vanilla, but does not remove bleeding effect from other hit zones of the group
	override void OnEffectRemoved(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		SCR_CharacterDamageManagerComponent characterDamageManager = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
		if (!characterDamageManager)
			return;
		
		characterDamageManager.RegenPhysicalHitZones();
		
		HitZone hitZone = GetAffectedHitZone();
		if (!hitZone)
			return;

		characterDamageManager.RemoveBleedingFromArray(hitZone);
		
		//Unsubscribe from SCR_BloodOnClothesSystem if registered
		if (s_System)
			s_System.Unregister(this);

		characterDamageManager.RemoveBleedingParticleEffect(hitZone);
	}
}
