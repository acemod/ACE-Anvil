//------------------------------------------------------------------------------------------------
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	//------------------------------------------------------------------------------------------------
	//! Release carried player if they wake up or if the carrier falls unconscious
	override protected void OnConsciousnessChanged(bool conscious)
	{
		super.OnConsciousnessChanged(conscious);
		ChimeraCharacter char = GetCharacter();
		ACE_Carrying_Helper.ReleaseFromCarrier(char);
		ACE_Carrying_Helper.ReleaseCarried(char);
	}
}
