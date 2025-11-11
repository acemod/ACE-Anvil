//------------------------------------------------------------------------------------------------
class ACE_Medical_SplintDamageEffect: SCR_DotDamageEffect
{
	//------------------------------------------------------------------------------------------------
	override EDamageType GetDefaultDamageType()
	{
		return EDamageType.HEALING;
	}
}
