//------------------------------------------------------------------------------------------------
//! Gadget Entity user action
class ACE_ContinousGadgetUserAction : ACE_BaseGadgetUserAction
{	
	//------------------------------------------------------------------------------------------------
	//! Stop animation when action is completed
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		CancelPlayerAnimation(pUserEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Start gadget animation
	override void OnActionStart(IEntity pUserEntity)
	{
		super.OnActionStart(pUserEntity);		
		TryUseGadget(pUserEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	//! End gadget animation
	override void OnActionCanceled(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.OnActionCanceled(pOwnerEntity, pUserEntity);
		
		ChimeraCharacter character = ChimeraCharacter.Cast(pUserEntity);
		if (!character)
			return;
		
		CharacterControllerComponent charController = character.GetCharacterController();
		if (charController)
		{
			CharacterAnimationComponent pAnimationComponent = charController.GetAnimationComponent();
			int itemActionId = pAnimationComponent.BindCommand("CMD_Item_Action");
			CharacterCommandHandlerComponent cmdHandler = CharacterCommandHandlerComponent.Cast(pAnimationComponent.GetCommandHandler());
			if (cmdHandler)
				cmdHandler.FinishItemUse(true);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! End gadget animation
	void CancelPlayerAnimation(IEntity user)
	{
		if (!user)
			return;
		
		ChimeraCharacter userChar = ChimeraCharacter.Cast(user);
		if (!userChar)
			return;
		
		SCR_CharacterControllerComponent userCharController = SCR_CharacterControllerComponent.Cast(userChar.GetCharacterController());
		if (!userCharController)
			return;
		
		CharacterAnimationComponent userAnimationComponent = userCharController.GetAnimationComponent();
		CharacterCommandHandlerComponent cmdHandler = CharacterCommandHandlerComponent.Cast(userAnimationComponent.GetCommandHandler());
		cmdHandler.FinishItemUse(true);
		
		if (m_pItemSoundComponent)
			userCharController.GetOnAnimationEvent().Remove(HandleItemSoundEvent);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Destructor - End Gadget animation
	void ~ACE_ContinousGadgetUserAction()
	{
		CancelPlayerAnimation(m_pUserChar);
	}
}
