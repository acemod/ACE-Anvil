//------------------------------------------------------------------------------------------------
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	//------------------------------------------------------------------------------------------------
	//! Release carried player if they wake up or if the carrier falls unconscious
	override protected void OnConsciousnessChanged(bool conscious)
	{
		super.OnConsciousnessChanged(conscious);
		ChimeraCharacter char = GetCharacter();
		CEN_CarrierSystem_Helper.ReleaseFromCarrier(char);
		CEN_CarrierSystem_Helper.ReleaseCarried(char);
	}
}
