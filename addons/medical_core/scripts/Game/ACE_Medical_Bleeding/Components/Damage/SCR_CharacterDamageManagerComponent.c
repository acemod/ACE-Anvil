//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	//-----------------------------------------------------------------------------------------------------------
	//! Applies bleed out settings
	//! Vanilla implementation handles bleed outs, so we only call it when bleed out is enabled
	override void UpdateConsciousness()
	{
		if (!EntityUtils.IsPlayer(GetOwner()) || m_pACE_Medical_Settings.m_bBleedOutForPlayersEnabled)
			return super.UpdateConsciousness();
		
		ChimeraCharacter character = ChimeraCharacter.Cast(GetOwner());
		if (!character)
			return;
		
		CharacterControllerComponent controller = character.GetCharacterController();
		if (!controller)
			return;

		controller.SetUnconscious(ShouldBeUnconscious());
	}
}
