[BaseContainerProps(), SCR_BaseGroupCommandTitleField("m_sCommandName")]
class ACE_ContinuousLoiterCommand : SCR_ContinuousLoiterCommand
{
	[Attribute(uiwidget: UIWidgets.ResourceNamePicker, desc: "Prefab name of the helper compatment", params: "et")]
	protected ResourceName m_pHelperCompatmentPrefabName;
	
	//------------------------------------------------------------------------------------------------
	override bool StartLoiter()
	{
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
			return false;
		
		playerController.ACE_RequestAnimateWithHelperCompartment(m_pHelperCompatmentPrefabName);
		return true;
	}
}
