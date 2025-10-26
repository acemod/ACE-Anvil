//-----------------------------------------------------------------------------
class ACE_TacticalPeriscope_GadgetComponentClass : SCR_BinocularsComponentClass
{
}


//-----------------------------------------------------------------------------
//! Same as binoculars, but gets a different camera in SCR_CharacterCameraHandlerComponent
class ACE_TacticalPeriscope_GadgetComponent : SCR_BinocularsComponent
{
	//-----------------------------------------------------------------------------
	override bool IsVisibleEquipped()
	{
		return false;
	}
}
