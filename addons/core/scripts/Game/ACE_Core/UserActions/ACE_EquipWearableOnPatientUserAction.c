//------------------------------------------------------------------------------------------------
class ACE_EquipWearableOnPatientUserAction : SCR_HealingUserAction
{
	protected SCR_ConsumableItemComponent m_ConsumableComponent;
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
			return false;
		
		SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(user);
		if (!userChar)
			return false;
		
		m_ConsumableComponent = GetConsumableComponent(userChar);
		if (!m_ConsumableComponent)
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!m_ConsumableComponent)
			return false;
		
		int reason;
		if (!m_ConsumableComponent.GetConsumableEffect().CanApplyEffect(GetOwner(), user, reason))
		{
			if (reason == SCR_EConsumableFailReason.UNDAMAGED)
				SetCannotPerformReason(m_sNotDamaged);		
			else if (reason == SCR_EConsumableFailReason.ALREADY_APPLIED)
				SetCannotPerformReason(m_sAlreadyApplied);
			
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
		
		if (!m_ConsumableComponent)
			return false;

		InventoryItemComponent item = InventoryItemComponent.Cast(m_ConsumableComponent.GetOwner().FindComponent(InventoryItemComponent));
		if (!item)
			return false;

		UIInfo itemInfo = item.GetUIInfo();
		if (!itemInfo)
			return false;

		outName = WidgetManager.Translate(actionInfo.GetName(), itemInfo.GetName());
		return true;
	}
}
