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
		
		vector hitPosDirNorm[3];
		SCR_DamageContext context = new SCR_DamageContext(
			effect.GetDamageType(), 
			effect.GetTotalDamage(), 
			hitPosDirNorm,
			dmgManager.GetOwner(), 
			affectedHitZone, 
			effect.GetInstigator(),
			null,
			-1,
			-1
		);
		
		float var = affectedHitZone.ComputeEffectiveDamage(context, false);
		if (var < affectedHitZone.GetCriticalHealthThreshold())
			return;
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(dmgManager.GetOwner());
		if (!ownerChar)
			return;
		
		ACE_Medical_RespiratoryComponent respiratoryComponent = ownerChar.ACE_Medical_GetRespiratoryComponent();
		if (!respiratoryComponent)
			return;
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (!settings || !settings.m_RespiratorySystem)
			return;
		
		respiratoryComponent.SetPneumothoraxVolume(settings.m_RespiratorySystem.m_fMaxPneumothoraxVolumeML / 4);
	}
}
