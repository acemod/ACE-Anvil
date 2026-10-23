//------------------------------------------------------------------------------------------------
class ACE_Medical_AITakeConsumableInLeftHand : SCR_AITakeGadgetInLeftHand
{
	[Attribute("0", UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(SCR_EConsumableType) )]
	private SCR_EConsumableType m_eConsumableItemType;
	
	// Inputs
	protected static const string PORT_CONSUMABLE_TYPE_IN = "ConsumableType";
	
	//------------------------------------------------------------------------------------------------
	protected static ref TStringArray s_aACE_VarsIn = { PORT_CONSUMABLE_TYPE_IN };
	override TStringArray GetVariablesIn() { return s_aACE_VarsIn; }
	
	//-----------------------------------------------------------------------------------------------
	override IEntity FindItem()
	{
		SCR_EConsumableType type;
		GetVariableIn(PORT_CONSUMABLE_TYPE_IN, type);
		if (!type)
			type = m_eConsumableItemType;
		
		if (type == SCR_EConsumableType.MED_KIT)
			return m_InventoryMgr.FindItem(new ACE_HealItemPredicate());
		
		ACE_ConsumableTypeWhitelistItemPredicate predicate = new ACE_ConsumableTypeWhitelistItemPredicate();
		predicate.m_aWhitelist = {type};
		return m_InventoryMgr.FindItem(predicate);
	}
}
