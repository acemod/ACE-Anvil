//------------------------------------------------------------------------------------------------
//! Move damage over time handling to ACE_Medical_BloodLossDamageEffect
modded class SCR_BleedingDamageEffect : SCR_DotDamageEffect
{
	//------------------------------------------------------------------------------------------------
	//! Register to ACE_Medical_BloodLossDamageEffect
	override void OnEffectAdded(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		super.OnEffectAdded(dmgManager);
		
		if (!Replication.IsServer())
			return;
		
		ACE_Medical_BloodLossDamageEffect bloodLossDamageEffect = ACE_Medical_BloodLossDamageEffect.Cast(dmgManager.FindDamageEffectOfType(ACE_Medical_BloodLossDamageEffect));
		if (!bloodLossDamageEffect)
		{
			// Add ACE_Medical_BloodLossDamageEffect if none is present
			dmgManager.AddDamageEffect(new ACE_Medical_BloodLossDamageEffect());
			bloodLossDamageEffect = ACE_Medical_BloodLossDamageEffect.Cast(dmgManager.FindDamageEffectOfType(ACE_Medical_BloodLossDamageEffect));
		}
		
		if (bloodLossDamageEffect)
			bloodLossDamageEffect.RegisterDamageEffect(dmgManager, this);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Unregister from ACE_Medical_BloodLossDamageEffect
	override void OnEffectRemoved(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		super.OnEffectRemoved(dmgManager);
		
		if (!Replication.IsServer())
			return;
		
		ACE_Medical_BloodLossDamageEffect bloodLossDamageEffect = ACE_Medical_BloodLossDamageEffect.Cast(dmgManager.FindDamageEffectOfType(ACE_Medical_BloodLossDamageEffect));
		if (bloodLossDamageEffect)
			bloodLossDamageEffect.UnregisterDamageEffect(dmgManager, this);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Disable damage over time, as ACE_Medical_BloodLossDamageEffect is responsible for it
	protected override void EOnFrame(float timeSlice, SCR_ExtendedDamageManagerComponent dmgManager)
	{
	}
}
