//------------------------------------------------------------------------------------------------
class ACE_Overheating_CoolBarrelWithFlaskFromInventoryUserAction : ACE_Overheating_CoolBarrelBaseUserAction
{
	[Attribute(defvalue: "#AR-Notification_PLAYER_FLASK_EMPTY", desc: "String for when flask is empty")]
	protected LocalizedString m_sFlaskEmpty;
	
	protected SCR_WaterContainerComponent m_WaterContainer;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_Item = InventoryItemComponent.Cast(pOwnerEntity.FindComponent(InventoryItemComponent));
		m_WaterContainer = SCR_WaterContainerComponent.Cast(pOwnerEntity.FindComponent(SCR_WaterContainerComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		ChimeraCharacter userChar = ChimeraCharacter.Cast(user);
		if(!userChar)
			return false;
		
		CharacterControllerComponent userCharController = userChar.GetCharacterController();
		if(!userCharController)
			return false;
		
		return userCharController.GetInspect();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (float.AlmostEqual(m_WaterContainer.GetCurrentWater(), 0.0))
		{
			SetCannotPerformReason(m_sFlaskEmpty);
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		UIInfo actionInfo = GetUIInfo();
		if (!actionInfo)
			return false;
		
		UIInfo itemInfo = m_Item.GetUIInfo();
		if (!itemInfo)
			return false;
		
		outName = WidgetManager.Translate(actionInfo.GetName(), itemInfo.GetName());
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPreformCoolingStep()
	{
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
			return;
		
		ChimeraCharacter userChar = ChimeraCharacter.Cast(playerController.GetLocalControlledEntity());
		if (!userChar)
			return;
		
		playerController.ACE_Overheating_RequestCoolBarrel(userChar.GetWeaponManager().GetCurrentWeapon(), m_WaterContainer);
	}
}
