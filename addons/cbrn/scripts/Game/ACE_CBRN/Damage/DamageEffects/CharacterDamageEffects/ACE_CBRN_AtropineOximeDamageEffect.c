//------------------------------------------------------------------------------------------------
class ACE_CBRN_AtropineOximeDamageEffect: SCR_DotDamageEffect
{
	//------------------------------------------------------------------------------------------------
	override bool HijackDamageEffect(SCR_ExtendedDamageManagerComponent dmgManager)
	{
		Debug.Error("Not implemented");
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override EDamageType GetDefaultDamageType()
	{
		return EDamageType.HEALING;
	}
}
