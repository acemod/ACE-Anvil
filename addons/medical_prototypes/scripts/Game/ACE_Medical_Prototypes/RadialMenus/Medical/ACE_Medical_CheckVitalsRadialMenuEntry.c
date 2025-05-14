//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true), SCR_BaseContainerCustomTitleUIInfo("Name")]
class ACE_Medical_CheckVitalsRadialMenuEntry : ACE_SelectionMenuEntry
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, desc: "Type of the notification", enums: ParamEnumArray.FromEnum(ENotification))]
	protected ENotification m_eNotificationType;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPerform()
	{
		PlayerCamera camera = PlayerCamera.Cast(GetGame().GetCameraManager().CurrentCamera());
		if (!camera)
			return;
		
		SCR_ChimeraCharacter targetChar = SCR_ChimeraCharacter.Cast(camera.GetCursorTarget());
		if (!targetChar)
			return;
		
		PlayerController playerController = GetGame().GetPlayerController();
		if (!playerController)
			return;
		
		ACE_Medical_NetworkComponent networkComponent = ACE_Medical_NetworkComponent.Cast(playerController.FindComponent(ACE_Medical_NetworkComponent));
		if (!networkComponent)
			return;
		
		networkComponent.RequestVitalsNotification(m_eNotificationType, targetChar);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformed()
	{
		PlayerCamera camera = GetGame().GetPlayerController().GetPlayerCamera();
		if (!camera)
			return false;
		
		return SCR_ChimeraCharacter.Cast(camera.GetCursorTarget());
	}
}
