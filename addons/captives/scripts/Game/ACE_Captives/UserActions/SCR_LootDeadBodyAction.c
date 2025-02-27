//------------------------------------------------------------------------------------------------
//! Allow looting surrendered and captive characters
modded class SCR_LootDeadBodyAction : SCR_LootAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (super.CanBePerformedScript(user))
			return true;
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar || ownerChar == user)
			return false;
		
		// Cannot loot inside vehicles
		if (ownerChar.IsInVehicle() && !ACE_AnimationTools.GetHelperCompartment(ownerChar))
			return false;
		
		SCR_CharacterControllerComponent ownerCharController = SCR_CharacterControllerComponent.Cast(ownerChar.GetCharacterController());
		if (!ownerCharController)
			return false;

		return ownerCharController.ACE_Captives_IsCaptive() || ownerCharController.ACE_Captives_HasSurrendered();
	}
}
