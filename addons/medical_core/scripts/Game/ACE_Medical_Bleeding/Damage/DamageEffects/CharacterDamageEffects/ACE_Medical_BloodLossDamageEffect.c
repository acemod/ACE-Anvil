//------------------------------------------------------------------------------------------------
//! Central effect responsible for applying all blood loss to SCR_CharacterBloodHitZone
class ACE_Medical_BloodLossDamageEffect : SCR_DotDamageEffect
{
	protected ref array<SCR_BleedingDamageEffect> m_aDamageEffects = {};
	
	//------------------------------------------------------------------------------------------------
	override bool HijackDamageEffect(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
		if (charDamageManager)
			SetAffectedHitZone(charDamageManager.GetBloodHitZone());
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void EOnFrame(float timeSlice, SCR_ExtendedDamageManagerComponent dmgManager)
	{
		DotDamageEffectTimerToken token = UpdateTimer(timeSlice, dmgManager);
		DealCustomDot(GetAffectedHitZone(), ComputeTotalBloodLoss(timeSlice, dmgManager), token, dmgManager);
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeTotalBloodLoss(float timeSlice, SCR_ExtendedDamageManagerComponent dmgManager)
	{
		SCR_CharacterBloodHitZone bloodHitZone = SCR_CharacterBloodHitZone.Cast(GetAffectedHitZone());
		if (!bloodHitZone)
			return 0;
		
		return bloodHitZone.GetTotalBleedingAmount() * timeSlice;
	}
	
	//------------------------------------------------------------------------------------------------
	override EDamageType GetDefaultDamageType()
	{
		return EDamageType.BLEEDING;
	}
}
