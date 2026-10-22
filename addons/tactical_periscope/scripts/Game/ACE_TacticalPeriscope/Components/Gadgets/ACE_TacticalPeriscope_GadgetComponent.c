//-----------------------------------------------------------------------------
class ACE_TacticalPeriscope_GadgetComponentClass : SCR_BinocularsComponentClass
{
}


//-----------------------------------------------------------------------------
//! Same as binoculars, but gets a different camera in SCR_CharacterCameraHandlerComponent
class ACE_TacticalPeriscope_GadgetComponent : SCR_BinocularsComponent
{
	//------------------------------------------------------------------------------------------------
	override protected void SetZoomedViewPlayer(bool value)
	{
		super.SetZoomedViewPlayer(value);
		SCR_PlayerController.s_pLocalPlayerController.ACE_SetIsPeriscopeZoomed(value);
	}
	
	//-----------------------------------------------------------------------------
	//! Hide when deposited in binocular storage slot
	override bool IsVisibleEquipped()
	{
		return false;
	}
}
