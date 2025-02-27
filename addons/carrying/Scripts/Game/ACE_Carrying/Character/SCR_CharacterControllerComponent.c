//------------------------------------------------------------------------------------------------
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	//------------------------------------------------------------------------------------------------
	void ACE_Carrying_CarryCasualty(SCR_ChimeraCharacter casualty)
	{
		ACE_AnimationHelperCompartment helper = ACE_AnimationTools.AnimateWithHelperCompartment(ACE_EAnimationHelperID.CARRIED, casualty);
		if (!helper)
			return;
		
		ACE_Carry(helper);
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Carrying_DragCasualty(SCR_ChimeraCharacter casualty)
	{
		ACE_AnimationHelperCompartment helper = ACE_AnimationTools.AnimateWithHelperCompartment(ACE_EAnimationHelperID.DRAGGED, casualty);
		if (!helper)
			return;
		
		ACE_Carry(helper);
	}
	
	//------------------------------------------------------------------------------------------------
	bool ACE_Carrying_CanCarryCasualty(SCR_ChimeraCharacter casualty, out string cannotPerformReason)
	{
		if (!ACE_Carrying_CanCarryOrDragCasualty(casualty, cannotPerformReason))
			return false;
		
		// Target stance is stand; Crouch is allowed if we can change to stand
		switch (GetStance())
		{
			case ECharacterStance.PRONE:
			{
				cannotPerformReason = "#AR-Keybind_StanceProne";
				return false;
			}
			
			case ECharacterStance.CROUCH:
			{
				cannotPerformReason = "#AR-Keybind_StanceCrouch";
				return CanChangeStance(ECharacterStanceChange.STANCECHANGE_TOERECTED);
			}
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool ACE_Carrying_CanDragCasualty(SCR_ChimeraCharacter casualty, out string cannotPerformReason)
	{
		if (!ACE_Carrying_CanCarryOrDragCasualty(casualty, cannotPerformReason))
			return false;
		
		// Target stance is crouch; Stand is allowed if we can change to crouch
		switch (GetStance())
		{
			case ECharacterStance.PRONE:
			{
				cannotPerformReason = "#AR-Keybind_StanceProne";
				return false;
			}
			
			case ECharacterStance.STAND:
			{
				cannotPerformReason = "#AR-Keybind_StanceStand";
				return CanChangeStance(ECharacterStanceChange.STANCECHANGE_TOCROUCH);
			}
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool ACE_Carrying_CanCarryOrDragCasualty(SCR_ChimeraCharacter casualty, out string cannotPerformReason)
	{
		if (!ACE_CanCarry(casualty, cannotPerformReason))
			return false;
		
		SCR_CharacterControllerComponent casualtyCharController = SCR_CharacterControllerComponent.Cast(casualty.GetCharacterController());
		if (!casualtyCharController || casualtyCharController.GetLifeState() != ECharacterLifeState.INCAPACITATED)
			return false;
		
		// Trying to carry while unit is ragdolling will break things
		if (casualtyCharController.GetAnimationComponent().IsRagdollActive())
		{
			cannotPerformReason = "#AR-UserActionUnavailable";
			return false;
		}
		
		return true;
	}
}
