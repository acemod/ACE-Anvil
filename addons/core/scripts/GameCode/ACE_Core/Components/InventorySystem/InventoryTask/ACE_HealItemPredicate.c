//------------------------------------------------------------------------------------------------
//! Match items with a SCR_HealSupportStationComponent
[BaseContainerProps()]
class ACE_HealItemPredicate : ACE_InventorySearchPredicate
{
	//------------------------------------------------------------------------------------------------
	void ACE_HealItemPredicate()
	{
		QueryComponentTypes.Insert(SCR_HealSupportStationComponent);
	}
}
