//------------------------------------------------------------------------------------------------
class ACE_Captives_TakeCaptiveUserAction : ACE_InstantGadgetUserAction
{
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;
		
		IEntity gadget = GetHeldGadget(user);
		if (!gadget)
			return false;
		
		if (!gadget.FindComponent(ACE_Captives_HandcuffsGadgetComponent))
			return false;
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar)
			return false;
		
		if (ownerChar.IsInVehicle() && !ACE_AnimationTools.GetHelperCompartment(ownerChar))
			return false;
		
		SCR_CharacterControllerComponent ownerCharController = SCR_CharacterControllerComponent.Cast(ownerChar.GetCharacterController());
		if (!ownerCharController)
			return false;
		
		if (ownerCharController.ACE_IsCarried())
			return false;
		
		if (ownerCharController.ACE_Captives_IsCaptive())
			return false;
		
		// Incapacitated characters can be tied
		if (ownerCharController.GetLifeState() == ECharacterLifeState.INCAPACITATED)
			return true;
				
		return ownerCharController.ACE_Captives_HasSurrendered();
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.PerformAction(pOwnerEntity, pUserEntity);
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(pOwnerEntity);
		if (!ownerChar)
			return;
		
		RplComponent rpl = ownerChar.GetRplComponent();
		if (!rpl)
			return;
		
		if (rpl.IsProxy())
			return;
		
		SCR_CharacterControllerComponent ownerCharController = SCR_CharacterControllerComponent.Cast(ownerChar.GetCharacterController());
		if (!ownerCharController)
			return;
		
		ownerCharController.ACE_Captives_SetCaptive(true);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected ItemUseParameters GetGadgetUseParams(IEntity user)
	{
		ItemUseParameters params = super.GetGadgetUseParams(user);
		if (!params)
			return null;
		
		params.SetKeepInHandAfterSuccess(false);
		return params;
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnGadgetUseEnded(IEntity item, bool successful, ItemUseParameters animParams)
	{
		super.OnGadgetUseEnded(item, successful, animParams);
		SCR_EntityHelper.DeleteEntityAndChildren(item);
	}
}
