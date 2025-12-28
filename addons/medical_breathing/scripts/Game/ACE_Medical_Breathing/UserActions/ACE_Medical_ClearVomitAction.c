//------------------------------------------------------------------------------------------------
class ACE_Medical_ClearVomitAction : ScriptedUserAction
{
	//------------------------------------------------------------------------------------------------
	//! Can this action be shown in the UI to the provided user entity?
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;

		SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(user);
		if (!userChar)
			return false;

		if (userChar.IsInVehicle())
			return false;

		CharacterControllerComponent userController = userChar.GetCharacterController();
		if (!userController)
			return false;

		if (userController.IsSwimming() || userController.IsFalling())
			return false;
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar)
			return false;
		
		if (ownerChar.IsInVehicle())
			return false;
		
		ACE_Medical_VitalsComponent ownerVitals = ACE_Medical_VitalsComponent.Cast(ownerChar.FindComponent(ACE_Medical_VitalsComponent));
		if (!ownerVitals)
			return false;
		
		return ownerVitals.IsAirwayOccluded();
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		ACE_Medical_VitalsComponent ownerVitals = ACE_Medical_VitalsComponent.Cast(pOwnerEntity.FindComponent(ACE_Medical_VitalsComponent));
		if (ownerVitals)
			ownerVitals.SetIsAirwayOccluded(false);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clients should not run the action
	override bool CanBroadcastScript()
	{
		return false;
	}
}
