class RON_CutFence : RON_UserActionPrefabReplaceSpawner
{	
		
	protected SCR_GadgetManagerComponent m_GadgetManager;
	
	//------------------------------------------------------------------------------------------------
	
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		super.PerformAction(pOwnerEntity, pUserEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{		
		if (!m_GadgetManager)
		{
			m_GadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(user);
			
			SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
			if (playerController)
				playerController.m_OnControlledEntityChanged.Insert(SetNewGadgetManager);
			
			return false;
		}
		
		SCR_GadgetComponent gadgetComponent = m_GadgetManager.GetHeldGadgetComponent();
		if (!gadgetComponent)
			return false;
		
		ChimeraCharacter character = ChimeraCharacter.Cast(user);
		CharacterControllerComponent charController = character.GetCharacterController();
		
		if (charController && !IsInProgress() && !charController.CanUseItem())
			return false;			
		
		return (gadgetComponent.GetMode() == EGadgetMode.IN_HAND);
	}
	
		
	override bool CanBeShownScript(IEntity user)
	{
		if (!m_GadgetManager)
		{
			m_GadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(user);
			
			SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
			if (playerController)
				playerController.m_OnControlledEntityChanged.Insert(SetNewGadgetManager);
			
			return false;
		}
		
		SCR_GadgetComponent gadgetComponent = m_GadgetManager.GetHeldGadgetComponent();
		if (!gadgetComponent)
			return false;
		
		return true;
	}
	//------------------------------------------------------------------------------------------------
	void SetNewGadgetManager(IEntity from, IEntity to)
	{
		m_GadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(to);
	}
	
	//------------------------------------------------------------------------------------------------
	void ~RON_CutFence()
	{
		
	}
};