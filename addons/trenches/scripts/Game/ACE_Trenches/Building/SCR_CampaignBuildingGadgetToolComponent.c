//------------------------------------------------------------------------------------------------
modded class SCR_CampaignBuildingGadgetToolComponent : SCR_GadgetComponent
{
	//------------------------------------------------------------------------------------------------
	//! Add sound for toggling placement mode
	override protected void SetPlacementMode(bool newState)
	{
		if (m_bPlacementModeEnabled != newState)
		{
			if (newState)
				SCR_UISoundEntity.SoundEvent("SOUND_E_PLACE_GSTART");
			else
				SCR_UISoundEntity.SoundEvent("SOUND_HUD_GADGET_CANCEL");
		}
		
		super.SetPlacementMode(newState);
	}
	
	//------------------------------------------------------------------------------------------------
	int ACE_Trenches_GetCurrentVariantID()
	{
		return m_iCurrentVariant;
	}
}
