//------------------------------------------------------------------------------------------------
modded class SCR_RemoveCasualtyUserAction : SCR_CompartmentUserAction
{
	//------------------------------------------------------------------------------------------------
	//! Initate carry when possible
	override protected void PerformRemoveCasualtyAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		// First call the base implementation, which ejects the character from the vehicle
		super.PerformRemoveCasualtyAction(pOwnerEntity, pUserEntity);
		
		BaseCompartmentSlot compartment = GetCompartmentSlot();
		if (!compartment)
			return;
		
		SCR_ChimeraCharacter casualtyChar = SCR_ChimeraCharacter.Cast(compartment.GetOccupant());
		if (!casualtyChar)
			return;
		
		SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!userChar)
			return;
		
		SCR_CharacterControllerComponent userCharController = SCR_CharacterControllerComponent.Cast(userChar.GetCharacterController());
		if (!userCharController)
			return;
		
		string cannotPerformReason;
		if (userCharController.ACE_Carrying_CanCarryCasualty(casualtyChar, cannotPerformReason))
			userCharController.ACE_Carrying_CarryCasualty(casualtyChar);
	}
}
