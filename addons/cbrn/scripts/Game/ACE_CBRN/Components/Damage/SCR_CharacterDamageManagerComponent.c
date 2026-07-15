//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	//------------------------------------------------------------------------------------------------
	//! Disable resilience regen when poisoned
	override void ACE_Medical_UpdateResilienceRegenScale()
	{
		super.ACE_Medical_UpdateResilienceRegenScale();
		
		if (FindDamageEffectOfType(SCR_PoisonDamageEffect))
			m_fACE_Medical_ResilienceRegenScale = 0;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Remove poison
	override void FullHeal(bool ignoreHealingDOT = true)
	{
		super.FullHeal(ignoreHealingDOT);
		TerminateDamageEffectsOfType(SCR_PoisonDamageEffect);
	}
}
