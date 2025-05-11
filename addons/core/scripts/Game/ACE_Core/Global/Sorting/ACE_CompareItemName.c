//---------------------------------------------------------------------------------------------------
class ACE_CompareItemName : SCR_SortCompare<IEntity>
{
	//---------------------------------------------------------------------------------------------------
	override static int Compare(IEntity left, IEntity right)
	{
		if (GetName(left).Compare(GetName(right)) == -1)
			return 1;
		else
			return 0;
	}
	
	//---------------------------------------------------------------------------------------------------
	protected static string GetName(IEntity item)
	{
		InventoryItemComponent itemComponent = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
		if (!itemComponent)
			return "";

		UIInfo uiInfo = itemComponent.GetUIInfo();
		if (!uiInfo)
			return "";
		
		return uiInfo.GetName();
	}
}
