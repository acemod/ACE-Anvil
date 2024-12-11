//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerEvaluator : DamageEffectEvaluator
{
	//------------------------------------------------------------------------------------------------
	override void HandleEffectConsequences(SCR_BaseBulletDamageEffect effect, ExtendedDamageManagerComponent dmgManager)
	{
		super.HandleEffectConsequences(effect, dmgManager);
		ACE_Medical_EffectCausePneumothorax(effect, dmgManager);
	}
	
	//------------------------------------------------------------------------------------------------
	override void HandleEffectConsequences(SCR_MeleeDamageEffect effect, ExtendedDamageManagerComponent dmgManager)
	{
		super.HandleEffectConsequences(effect, dmgManager);
		ACE_Medical_EffectCausePneumothorax(effect, dmgManager);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	protected void ACE_Medical_EffectCausePneumothorax(SCR_InstantDamageEffect effect, ExtendedDamageManagerComponent dmgManager)
	{
		ACE_Medical_ChestHitZone affectedHitZone = ACE_Medical_ChestHitZone.Cast(effect.GetAffectedHitZone());
		if (!affectedHitZone)
			return;
		
		if (effect.GetTotalDamage() < affectedHitZone.GetCriticalDamageThreshold() * affectedHitZone.GetMaxHealth())
			return;
		
		ACE_Medical_RespiratoryComponent respiratoryComponent = ACE_Medical_RespiratoryComponent.Cast(dmgManager.GetOwner().FindComponent(ACE_Medical_RespiratoryComponent));
		if (!respiratoryComponent)
			return;
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (!settings || !settings.m_RespiratorySystem)
			return;
		
		respiratoryComponent.SetPneumothoraxVolume(settings.m_RespiratorySystem.m_fMaxPneumothoraxVolumeML / 4);
	}
}
