//------------------------------------------------------------------------------------------------
//! Wrench Entity user action
class ACE_WrenchUserAction : ACE_GadgetUserAction
{	
	//------------------------------------------------------------------------------------------------
	//! User needs to equip the Wrench for the action to show up
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;
					
		IEntity gadget = m_GadgetManager.GetHeldGadget();
		if (!gadget)
            return false;
		
         if (!gadget.FindComponent(SCR_RepairSupportStationComponent))
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
