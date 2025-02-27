[BaseContainerProps(), SCR_BaseGroupCommandTitleField("m_sCommandName")]
class ACE_ContinuousLoiterCommand : SCR_ContinuousLoiterCommand
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, desc: "ID of the animation helper",  enums: ParamEnumArray.FromEnum(ACE_EAnimationHelperID))]
	protected ACE_EAnimationHelperID m_eAnimationHelperID;
	
	//------------------------------------------------------------------------------------------------
	override bool StartLoiter()
	{
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
			return false;
		
		playerController.ACE_RequestAnimateWithHelperCompartment(m_eAnimationHelperID);
		return true;
	}
}
