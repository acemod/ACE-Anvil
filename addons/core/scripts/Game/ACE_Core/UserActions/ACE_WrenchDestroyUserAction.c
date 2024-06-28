//------------------------------------------------------------------------------------------------
//! Base Entity deletion user action
class ACE_WrenchDestroyUserAction : ScriptedUserAction
{
	protected SCR_GadgetManagerComponent m_GadgetManager;
	protected IEntity m_pUser;
	
	//------------------------------------------------------------------------------------------------
	//! Start Wrench animation
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
			params.SetEntity(GetBuildingTool(pUserEntity));
			params.SetAllowMovementDuringAction(false);
			params.SetKeepInHandAfterSuccess(true);
			params.SetCommandID(itemActionId);
			params.SetCommandIntArg(2);
			
			charController.TryUseItemOverrideParams(params);
		}
		
		m_pUser = pUserEntity;
	}
	
	//------------------------------------------------------------------------------------------------
	//! End Wrench animation
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
				cmdHandler.FinishItemUse();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! End Wrench animation
	void CancelPlayerAnimation(IEntity entity)
	{
		if (!entity)
			return;
		
		ChimeraCharacter character = ChimeraCharacter.Cast(entity);
		if (!character)
			return;
		
		CharacterControllerComponent charController = character.GetCharacterController();
		if (charController)
		{
			CharacterAnimationComponent pAnimationComponent = charController.GetAnimationComponent();
			CharacterCommandHandlerComponent cmdHandler = CharacterCommandHandlerComponent.Cast(pAnimationComponent.GetCommandHandler());
			cmdHandler.FinishItemUse();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get Wrench entity
	IEntity GetBuildingTool(notnull IEntity ent)
	{
		SCR_GadgetManagerComponent gadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(ent);
		if (!gadgetManager)
			return null;
		
		return gadgetManager.GetHeldGadget();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Sets a new gadget manager. Controlled by an event when the controlled entity has changed.
	void SetNewGadgetManager(IEntity from, IEntity to)
	{
		m_GadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(to);
	}
	
	//------------------------------------------------------------------------------------------------
	//! User needs to equip the Wrench for the action to show up
	override bool CanBeShownScript(IEntity user)
	{
		if (!m_GadgetManager)
		{
			m_GadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(user);
			
			SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
			if (playerController)
				playerController.m_OnControlledEntityChanged.Insert(SetNewGadgetManager);
			
			return false;
		};
					
		SCR_GadgetComponent gadget = m_GadgetManager.GetHeldGadgetComponent();
		if (!gadget)
            return false;
		
         if (!gadget.FindComponent(SCR_RepairSupportStationComponent))
            return false;
		
		 return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Destructor - End Wrench animation
	void ~ACE_WrenchDestroyUserAction()
	{
		CancelPlayerAnimation(m_pUser);
	}
}