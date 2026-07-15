//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	//------------------------------------------------------------------------------------------------
	//! Remove poison
	override void FullHeal(bool ignoreHealingDOT = true)
	{
		super.FullHeal(ignoreHealingDOT);
		TerminateDamageEffectsOfType(SCR_PoisonDamageEffect);
	}
}
