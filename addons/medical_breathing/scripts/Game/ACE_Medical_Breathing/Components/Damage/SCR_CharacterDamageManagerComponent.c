//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	//-----------------------------------------------------------------------------------------------------------
	//! TO DO: Remove solution once systems support inheritance
	override void FullHeal(bool ignoreHealingDOT = true)
	{
		super.FullHeal(ignoreHealingDOT);
		
		ACE_Medical_BaseSystem3 system3 = ACE_Medical_BaseSystem3.GetInstance(ACE_Medical_RespiratorySystem);
		if (system3)
			system3.OnFullHeal(SCR_ChimeraCharacter.Cast(GetOwner()));
	}
	//-----------------------------------------------------------------------------------------------------------
}
