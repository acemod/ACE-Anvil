//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class ACE_Medical_ExamineCasualtyRadialMenuCategoryEntry : ACE_SelectionMenuCategoryEntry
{
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed()
	{
		PlayerCamera camera = GetGame().GetPlayerController().GetPlayerCamera();
		if (!camera)
			return false;
		
		return SCR_ChimeraCharacter.Cast(camera.GetCursorTarget());
	}
}
