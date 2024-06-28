//------------------------------------------------------------------------------------------------
//! Wrench Entity user action
class ACE_WrenchUserAction : ACE_GadgetUserAction
{	
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