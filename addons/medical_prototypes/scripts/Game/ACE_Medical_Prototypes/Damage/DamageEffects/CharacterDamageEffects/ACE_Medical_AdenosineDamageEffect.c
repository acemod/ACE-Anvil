//------------------------------------------------------------------------------------------------
class ACE_Medical_AdenosineDamageEffect : SCR_DotDamageEffect
{
	//------------------------------------------------------------------------------------------------
	override bool HijackDamageEffect(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		SCR_CharacterDamageManagerComponent charDmgManager = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
		if (charDmgManager)
			SetAffectedHitZone(charDmgManager.GetResilienceHitZone());
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override EDamageType GetDefaultDamageType()
	{
		return EDamageType.HEALING;
	}
}
