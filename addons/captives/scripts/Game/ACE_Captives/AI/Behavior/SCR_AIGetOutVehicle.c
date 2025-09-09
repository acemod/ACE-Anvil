//------------------------------------------------------------------------------------------------
//! Prevent captive AI from exiting vehicles
modded class SCR_AIGetOutVehicle : SCR_AIVehicleBehavior
{
	//------------------------------------------------------------------------------------------------
	override void OnActionSelected()
	{
		ChimeraCharacter char = ChimeraCharacter.Cast(m_Utility.m_OwnerEntity);
		if (!char)
			return Fail();
		
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(char.GetCharacterController());
		if (!charController || charController.ACE_Captives_IsCaptive())
			return Fail();
		
		super.OnActionSelected();
	}
}

