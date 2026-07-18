//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	protected static ACE_CBRN_Settings s_ACE_CBRN_Settings;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Initialize members
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
				
		if (!s_ACE_CBRN_Settings)
			s_ACE_CBRN_Settings = ACE_SettingsHelperT<ACE_CBRN_Settings>.GetModSettings();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Remove poison
	override void FullHeal(bool ignoreHealingDOT = true)
	{
		super.FullHeal(ignoreHealingDOT);
		TerminateDamageEffectsOfType(SCR_PoisonDamageEffect);
	}
}
