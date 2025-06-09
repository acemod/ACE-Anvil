//------------------------------------------------------------------------------------------------
//! Move damage over time handling to ACE_Medical_BloodLossDamageEffect
modded class SCR_BleedingDamageEffect : SCR_DotDamageEffect
{
	//------------------------------------------------------------------------------------------------
	//! Start ACE_Medical_BloodLossDamageEffect if none is present
	override void OnEffectAdded(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		super.OnEffectAdded(dmgManager);
		
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
		if (charDamageManager)
			charDamageManager.GetBloodHitZone().ACE_Medical_UpdateTotalBleedingAmount();
		
		if (dmgManager.FindDamageEffectOfType(ACE_Medical_BloodLossDamageEffect))
			return;
		
		ACE_Medical_BloodLossDamageEffect bloodLossDamageEffect = new ACE_Medical_BloodLossDamageEffect();
		bloodLossDamageEffect.SetMaxDuration(0);
		bloodLossDamageEffect.SetInstigator(GetInstigator());
		dmgManager.AddDamageEffect(bloodLossDamageEffect);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminate ACE_Medical_BloodLossDamageEffect if no bleedings are left
	override void OnEffectRemoved(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		super.OnEffectRemoved(dmgManager);
		
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
		if (charDamageManager)
			charDamageManager.GetBloodHitZone().ACE_Medical_UpdateTotalBleedingAmount();
		
		if (!dmgManager.FindDamageEffectOfType(SCR_BleedingDamageEffect))
			dmgManager.TerminateDamageEffectsOfType(ACE_Medical_BloodLossDamageEffect);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Disable damage over time, as ACE_Medical_BloodLossDamageEffect is responsible for it
	protected override void EOnFrame(float timeSlice, SCR_ExtendedDamageManagerComponent dmgManager)
	{
	}
	
	//------------------------------------------------------------------------------------------------
	//! We update bleeding amount here as well, since when the hit zone already has a bleeding effect,
	//! adding another one gets hijacked and the DPS gets updated instead.
	override bool HijackDamageEffect(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		if (!super.HijackDamageEffect(dmgManager))
			return false;
		
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
		if (charDamageManager)
			charDamageManager.GetBloodHitZone().ACE_Medical_UpdateTotalBleedingAmount();
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Refactored calculation to SCR_CharacterHitZone::CalculateBleedingRate
	override float CalculateBleedingRate()
	{
		SCR_CharacterHitZone affectedHitZone = SCR_CharacterHitZone.Cast(GetAffectedHitZone());
		if (!affectedHitZone)
		{
			Print("SCR_BleedingDamageEffect instance without hitZone exists", LogLevel.ERROR);
			return 0;
		}

		return affectedHitZone.ACE_Medical_CalculateBleedingRate();
	}
}
