//------------------------------------------------------------------------------------------------
//! Match items with a SCR_HealSupportStationComponent
[BaseContainerProps()]
modded class ACE_HealItemPredicate : ACE_InventorySearchPredicate
{
	//------------------------------------------------------------------------------------------------
	void ACE_HealItemPredicate()
	{
		QueryComponentTypes = {SCR_GadgetComponent};
	}
	
	//------------------------------------------------------------------------------------------------
	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		SCR_SupportStationGadgetComponent supportStation = SCR_SupportStationGadgetComponent.Cast(queriedComponents[0]);
		if (supportStation && supportStation.IsGadgetOfSupportStationType(ESupportStationType.HEAL))
			return true;
		
		SCR_ConsumableItemComponent consumableComponent = SCR_ConsumableItemComponent.Cast(queriedComponents[0]);
		if (consumableComponent && consumableComponent.GetConsumableType() == SCR_EConsumableType.ACE_MEDICAL_SPLINT)
			return true;
		
		return false;
	}
}
