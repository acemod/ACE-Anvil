//------------------------------------------------------------------------------------------------
//! Add action for removing captives
modded class SCR_RemoveCasualtyUserAction : SCR_CompartmentUserAction
{
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (CanRemoveCaptiveBeShown(pUserEntity) && CanRemoveCaptiveBePeformed(pUserEntity))
		{
			PerformRemoveCaptiveAction(pOwnerEntity, pUserEntity);
			return;
		}
		
		super.PerformAction(pOwnerEntity, pUserEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void PerformRemoveCaptiveAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		BaseCompartmentSlot compartment = GetCompartmentSlot();
		if (!compartment)
			return;
		
		SCR_ChimeraCharacter captiveChar = SCR_ChimeraCharacter.Cast(compartment.GetOccupant());
		if (!captiveChar)
			return;
		
		SCR_CompartmentAccessComponent captiveCompartmentAccess = SCR_CompartmentAccessComponent.Cast(captiveChar.GetCompartmentAccessComponent());
		if (captiveCompartmentAccess)
			captiveCompartmentAccess.KickFromVehicle(GetRelevantDoorIndex(pUserEntity));
		
		ACE_AnimationHelperCompartment helper = ACE_AnimationTools.AnimateWithHelperCompartment(ACE_EAnimationHelperID.TIED, captiveChar);
		if (!helper)
			return;
		
		SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(pUserEntity);
		if (!userChar)
			return;
		
		SCR_CharacterControllerComponent userCharController = SCR_CharacterControllerComponent.Cast(userChar.GetCharacterController());
		if (!userCharController)
			return;
		
		userCharController.ACE_Carry(helper);
	}
		
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		IEntity user = SCR_PlayerController.GetLocalControlledEntity();
		if (CanRemoveCaptiveBeShown(user) && CanRemoveCaptiveBePeformed(user))
			return GetRemoveCaptiveActionName(outName);
		
		return super.GetActionNameScript(outName);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool GetRemoveCaptiveActionName(out string outName)
	{
		BaseCompartmentSlot compartment = GetCompartmentSlot();
		if (!compartment)
			return false;
		
		UIInfo compartmentInfo = compartment.GetUIInfo();
		if (!compartmentInfo)
			return false;
				
		outName = "#ACE-UserAction_LoadCarried" + "%CTX_HACK%" + compartmentInfo.GetName();
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (super.CanBeShownScript(user))
			return true;
		
		return CanRemoveCaptiveBeShown(user);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool CanRemoveCaptiveBeShown(IEntity user)
	{
		SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(user);
		if (!userChar || userChar.IsInVehicle())
			return false;
		
		BaseCompartmentSlot compartment = GetCompartmentSlot();
		if (!compartment)
			return false;
		
		SCR_ChimeraCharacter captiveChar = SCR_ChimeraCharacter.Cast(compartment.GetOccupant());
		if (!captiveChar)
			return false;
		
		SCR_CharacterControllerComponent captiveCharController =  SCR_CharacterControllerComponent.Cast(captiveChar.GetCharacterController());
		if (!captiveCharController)
			return false;
		
		if (captiveCharController.GetLifeState() != ECharacterLifeState.ALIVE)
			return false;
				
		return captiveCharController.ACE_Captives_IsCaptive();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (super.CanBePerformedScript(user))
			return true;
		
		return CanRemoveCaptiveBePeformed(user);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool CanRemoveCaptiveBePeformed(IEntity user)
	{
		SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(user);
		if (!userChar)
			return false;
		
		// Cannot be performed when already carrying
		SCR_CharacterControllerComponent userCharController = SCR_CharacterControllerComponent.Cast(userChar.GetCharacterController());
		if (!userCharController || userCharController.ACE_IsCarrier())
		{
			SetCannotPerformReason("#ACE-UserAction_Carrying");
			return false;
		}
		
		SCR_CompartmentAccessComponent compartmentAccess = SCR_CompartmentAccessComponent.Cast(userChar.GetCompartmentAccessComponent());
		if (!compartmentAccess)
			return false;
		
		BaseCompartmentSlot compartment = GetCompartmentSlot();
		if (!compartment)
			return false;
		
		return true;
	}
}
