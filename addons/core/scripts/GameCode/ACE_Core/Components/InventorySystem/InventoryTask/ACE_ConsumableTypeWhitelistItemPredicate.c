//------------------------------------------------------------------------------------------------
//! Match items based on a whitelist of ECommonItemType
[BaseContainerProps()]
class ACE_ConsumableTypeWhitelistItemPredicate : ACE_InventorySearchPredicate
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(SCR_EConsumableType))]
	ref array<SCR_EConsumableType> m_aWhitelist;

	//------------------------------------------------------------------------------------------------
	void ACE_ConsumableTypeWhitelistItemPredicate()
	{
		QueryComponentTypes.Insert(SCR_ConsumableItemComponent);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		SCR_ConsumableItemComponent cosumableComponent = SCR_ConsumableItemComponent.Cast(queriedComponents[0]);
		return m_aWhitelist.Find(cosumableComponent.GetConsumableType()) >= 0;
	}
}
