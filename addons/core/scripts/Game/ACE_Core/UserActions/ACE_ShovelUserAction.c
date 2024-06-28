//------------------------------------------------------------------------------------------------
//! Shovel Entity user action
class ACE_ShovelUserAction : ACE_GadgetUserAction
{
	//------------------------------------------------------------------------------------------------
	//! User needs to equip the shovel for the action to show up
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
					
		if (!SCR_CampaignBuildingGadgetToolComponent.Cast(m_GadgetManager.GetHeldGadgetComponent()))
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Destructor - End E-tool animation
	void ~ACE_ShovelUserAction()
	{
		CancelPlayerAnimation(m_pUser);
	}
}