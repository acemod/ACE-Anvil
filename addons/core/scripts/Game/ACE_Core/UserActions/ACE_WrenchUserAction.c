//------------------------------------------------------------------------------------------------
//! Wrench Entity user action
class ACE_WrenchUserAction : ACE_GadgetUserAction
{	
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
			params.SetCommandIntArg(1);
			
			charController.TryUseItemOverrideParams(params);
		}
		
		m_pUser = pUserEntity;
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
		
         if (!gadget.GetOwner().FindComponent(SCR_RepairSupportStationComponent))
            return false;
		
		 return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Destructor - End Wrench animation
	void ~ACE_WrenchUserAction()
	{
		CancelPlayerAnimation(m_pUser);
	}
}