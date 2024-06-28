//------------------------------------------------------------------------------------------------
//! Shovel Entity user action
class ACE_ShovelUserAction : ACE_GadgetUserAction
{	
	//------------------------------------------------------------------------------------------------
	//! User needs to equip the shovel for the action to show up
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;
					
		if (!SCR_CampaignBuildingGadgetToolComponent.Cast(m_GadgetManager.GetHeldGadgetComponent()))
			return false;
		
		return true;
	}
}
