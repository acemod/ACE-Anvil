//------------------------------------------------------------------------------------------------
modded class SCR_InteractionHandlerComponent
{
	//------------------------------------------------------------------------------------------------
	//! Enable when patient inspection is active
	override protected bool ShouldBeEnabled(SCR_NearbyContextDisplayMode displayMode, ChimeraCharacter character, bool playerCameraOnly = true)
	{
		if (super.ShouldBeEnabled(displayMode, character, playerCameraOnly))
			return true;

		SCR_HUDManagerComponent hudManager = SCR_HUDManagerComponent.GetHUDManager();
		if (!hudManager)
			return false;

		ACE_Medical_InspectionDisplay display = ACE_Medical_InspectionDisplay.Cast(hudManager.FindInfoDisplay(ACE_Medical_InspectionDisplay));
		if (!display)
			return false;

		return display.IsShown();
	}
}
