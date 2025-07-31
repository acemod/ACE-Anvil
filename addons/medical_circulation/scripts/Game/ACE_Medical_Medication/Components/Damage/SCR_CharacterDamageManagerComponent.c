//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	//-----------------------------------------------------------------------------------------------------------
	//! Clear medication when fully healed (e.g. by GM)
	override void FullHeal(bool ignoreHealingDOT = true)
	{
		super.FullHeal(ignoreHealingDOT);
		
		ACE_Medical_MedicationComponent medicationComponent = ACE_Medical_MedicationComponent.Cast(GetOwner().FindComponent(ACE_Medical_MedicationComponent));
		if (medicationComponent)
			medicationComponent.Clear();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Unregister from medical systems when killed
	override void ACE_Medical_OnKilled()
	{
		super.ACE_Medical_OnKilled();
		
		ACE_Medical_MedicationSystem system = ACE_Medical_MedicationSystem.GetInstance();
		if (system)
			system.Unregister(SCR_ChimeraCharacter.Cast(GetOwner()));
	}
}
