//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerEvaluator : DamageEffectEvaluator
{
	//------------------------------------------------------------------------------------------------
	//! Suppress removal of bleeding effect on other hit zones of the group
	override void HandleEffectConsequences(SCR_BandageDamageEffect effect, ExtendedDamageManagerComponent dmgManager)
	{
	}
}
