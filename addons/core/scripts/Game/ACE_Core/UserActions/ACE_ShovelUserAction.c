//------------------------------------------------------------------------------------------------
//! Shovel Entity user action
class ACE_ShovelUserAction : ACE_GadgetUserAction
{	
	//------------------------------------------------------------------------------------------------
	//! User needs to equip the shovel for the action to show up
	override bool CanBeShownScript(IEntity user)
	{
		IEntity gadget = GetHeldGadget(user);
		if (!gadget)
            return false;
		
        if (!gadget.FindComponent(SCR_CampaignBuildingGadgetToolComponent))
        	return false;
		
		return true;
	}
}
