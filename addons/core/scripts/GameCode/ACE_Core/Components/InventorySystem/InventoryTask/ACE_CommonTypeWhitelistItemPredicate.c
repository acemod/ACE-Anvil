//------------------------------------------------------------------------------------------------
//! Match items based on a whitelist of ECommonItemType
[BaseContainerProps()]
class ACE_CommonTypeWhitelistItemPredicate : ACE_InventorySearchPredicate
{
	[Attribute(defvalue: "", uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(ECommonItemType))]
	ref array<ECommonItemType> m_aWhitelist;

	//------------------------------------------------------------------------------------------------
	void ACE_CommonTypeWhitelistItemPredicate()
	{
		QueryAttributeTypes.Insert(SCR_ItemOfInterestAttribute);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		SCR_ItemOfInterestAttribute optionalAttribute = SCR_ItemOfInterestAttribute.Cast(queriedAttributes[0]);
		return m_aWhitelist.Find(optionalAttribute.GetInterestType()) >= 0;
	}
}
