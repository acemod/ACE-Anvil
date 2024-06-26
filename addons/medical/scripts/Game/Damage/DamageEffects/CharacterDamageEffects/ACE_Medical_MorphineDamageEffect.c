//------------------------------------------------------------------------------------------------
class ACE_Medical_MorphineDamageEffect: SCR_DotDamageEffect
{
	//------------------------------------------------------------------------------------------------
	override bool HijackDamageEffect(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		const SCR_CharacterDamageManagerComponent charDmgManager = SCR_CharacterDamageManagerComponent.Cast(dmgManager);
		if (charDmgManager)
			SetAffectedHitZone(charDmgManager.ACE_Medical_GetPainHitZone());
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override EDamageType GetDefaultDamageType()
	{
		return EDamageType.HEALING;
	}
}
