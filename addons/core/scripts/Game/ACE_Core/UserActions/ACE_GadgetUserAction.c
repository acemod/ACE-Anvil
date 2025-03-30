//------------------------------------------------------------------------------------------------
//! Gadget Entity user action
class ACE_GadgetUserAction : ScriptedUserAction
{
	[Attribute(defvalue: "1", desc: "Index of the gadget animation to play")]
	protected int m_iAnimationIndex;
	
	protected IEntity m_pUser;
	
	//------------------------------------------------------------------------------------------------
	//! Stop animation when action is completed
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.PerformAction(pOwnerEntity, pUserEntity);
		CancelPlayerAnimation(pUserEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Start gadget animation
	override void OnActionStart(IEntity pUserEntity)
	{
		super.OnActionStart(pUserEntity);
		
		ChimeraCharacter character = ChimeraCharacter.Cast(pUserEntity);
		if (!character)
			return;
		
		CharacterControllerComponent charController = character.GetCharacterController();
		if (charController)
		{
			CharacterAnimationComponent pAnimationComponent = charController.GetAnimationComponent();
			int itemActionId = pAnimationComponent.BindCommand("CMD_Item_Action");
			ItemUseParameters params = new ItemUseParameters();
			params.SetEntity(GetHeldGadget(pUserEntity));
			params.SetAllowMovementDuringAction(false);
			params.SetKeepInHandAfterSuccess(true);
			params.SetCommandID(itemActionId);
			params.SetCommandIntArg(GetAnimationIndex());
			
			charController.TryUseItemOverrideParams(params);
		}
		
		m_pUser = pUserEntity;
	}
	
	//------------------------------------------------------------------------------------------------
	//! End gadget animation
	override void OnActionCanceled(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		super.OnActionCanceled(pOwnerEntity, pUserEntity);
		CancelPlayerAnimation(pUserEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	//! End gadget animation
	void CancelPlayerAnimation(IEntity entity)
	{
		if (!entity)
			return;
		
		ChimeraCharacter character = ChimeraCharacter.Cast(entity);
		if (!character)
			return;
		
		CharacterControllerComponent charController = character.GetCharacterController();
		if (!charController)
			return;
		CharacterAnimationComponent animationComponent = charController.GetAnimationComponent();
		if (!animationComponent)
			return;
		
		CharacterCommandHandlerComponent cmdHandler = CharacterCommandHandlerComponent.Cast(animationComponent.GetCommandHandler());
		if (cmdHandler)
			cmdHandler.FinishItemUse(true);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get gadget entity
	IEntity GetHeldGadget(notnull IEntity ent)
	{
		SCR_GadgetManagerComponent gadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(ent);
		if (!gadgetManager)
			return null;
		
		return gadgetManager.GetHeldGadget();
	}
	
	//------------------------------------------------------------------------------------------------
	int GetAnimationIndex()
	{
		return m_iAnimationIndex;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Destructor - End Gadget animation
	void ~ACE_GadgetUserAction()
	{
		CancelPlayerAnimation(m_pUser);
	}
}
