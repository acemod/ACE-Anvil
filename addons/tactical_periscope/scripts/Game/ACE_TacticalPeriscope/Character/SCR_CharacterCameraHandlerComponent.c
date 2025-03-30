//------------------------------------------------------------------------------------------------
//! Add handling for periscopic camera
modded class SCR_CharacterCameraHandlerComponent : CameraHandlerComponent
{
	//------------------------------------------------------------------------------------------------
	//! Select periscopic camera when one is in zoom view
	override int CameraSelector()
	{
		if (ACE_TacticalPeriscope_GadgetComponent.IsZoomedView())
			return CharacterCameraSet.ACE_TACTICAL_PERISCOPE_CHARACTERCAMERA;
		
		return super.CameraSelector();
	}
	
	//------------------------------------------------------------------------------------------------
	//! No transition effect for periscopic cameras
	override float GetCameraTransitionTime(int pFrom, int pTo)
	{
		if (m_ControllerComponent && (pFrom == CharacterCameraSet.ACE_TACTICAL_PERISCOPE_CHARACTERCAMERA || pTo == CharacterCameraSet.ACE_TACTICAL_PERISCOPE_CHARACTERCAMERA))
			return 0;
		
		return super.GetCameraTransitionTime(pFrom, pTo);
	}
}
