//------------------------------------------------------------------------------------------------
class ACE_CBRN_AtropineOximeDamageEffect : SCR_DotDamageEffect
{
	//------------------------------------------------------------------------------------------------
	override bool HijackDamageEffect(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		SCR_CharacterDamageManagerComponent charDmgManager = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
		if (charDmgManager)
			SetAffectedHitZone(charDmgManager.ACE_Medical_GetNervousSystemHitZone());
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override EDamageType GetDefaultDamageType()
	{
		return EDamageType.HEALING;
	}
}
