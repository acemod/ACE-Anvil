//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	//-----------------------------------------------------------------------------------------------------------
	//! Refactored bleed out handling to SCR_CharacterBloodHitZone::OnDamageStateChanged
	override void UpdateConsciousness()
	{
		ChimeraCharacter character = ChimeraCharacter.Cast(GetOwner());
		if (!character)
			return;
		
		CharacterControllerComponent controller = character.GetCharacterController();
		if (!controller)
			return;

		controller.SetUnconscious(ShouldBeUnconscious());
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetTourniquettedGroup(ECharacterHitZoneGroup hitZoneGroup, bool setTourniquetted)
	{
		super.SetTourniquettedGroup(hitZoneGroup, setTourniquetted);
		m_pBloodHitZone.ACE_Medical_UpdateTotalBleedingAmount();
	}
}
