//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class ACE_Medical_InspectCasualtyRadialMenuEntry : ACE_SelectionMenuEntry
{
	//------------------------------------------------------------------------------------------------
	override protected void OnPerform()
	{
		PlayerCamera camera = PlayerCamera.Cast(GetGame().GetCameraManager().CurrentCamera());
		if (!camera)
			return;
		
		SCR_ChimeraCharacter targetChar = SCR_ChimeraCharacter.Cast(camera.GetCursorTarget());
		if (!targetChar)
			return;
		
		array<BaseInfoDisplay> displays = {};
		GetGame().GetPlayerController().GetHUDManagerComponent().GetInfoDisplays(displays);
		
		foreach (BaseInfoDisplay display : displays)
		{
			SCR_InspectCasualtyWidget casualtyInspectDisplay = SCR_InspectCasualtyWidget.Cast(display);
			if (!casualtyInspectDisplay)
				continue;

			casualtyInspectDisplay.SetTarget(targetChar);
			casualtyInspectDisplay.ShowInspectCasualtyWidget(targetChar);
			return;
		}
	}
}
