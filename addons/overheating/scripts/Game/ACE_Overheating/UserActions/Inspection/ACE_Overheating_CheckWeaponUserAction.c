//------------------------------------------------------------------------------------------------
class ACE_Overheating_CheckWeaponUserAction : SCR_InspectionUserAction
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, desc: "Type of the notification", enums: ParamEnumArray.FromEnum(ENotification))]
	protected ENotification m_eNotificationType;
	
	//------------------------------------------------------------------------------------------------
	override protected void PerformActionInternal(SCR_InventoryStorageManagerComponent manager, IEntity pOwnerEntity, IEntity pUserEntity)
	{
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (playerController)
			playerController.ACE_Overheating_RequestWeaponStateNotification(m_eNotificationType, m_WeaponComponent);
	}
}
