//------------------------------------------------------------------------------------------------
//! Wrench Entity user action
class ACE_WrenchUserAction : ACE_ContinousGadgetUserAction
{	
	//------------------------------------------------------------------------------------------------
	//! User needs to equip the wrench for the action to show up
	override bool CanBeShownScript(IEntity user)
	{
		IEntity gadget = GetHeldGadget(user);
		if (!gadget)
            return false;
		
        if (!gadget.FindComponent(SCR_RepairSupportStationComponent))
    		return false;
		
		return true;
	}
}
