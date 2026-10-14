class ACE_Inventory_Settings
{
	static const int SMALL_BATCH_SIZE = 5;
	static const int LARGE_BATCH_SIZE = 10;
	static const int ALL_ARSENAL_LIMIT = 100;
	static const int ALL_ITEMS = -1;

	static bool IsSupportedQuantity(int quantity)
	{
		return quantity == ALL_ITEMS || (quantity > 0 && (quantity == SMALL_BATCH_SIZE || quantity == LARGE_BATCH_SIZE));
	}
}
