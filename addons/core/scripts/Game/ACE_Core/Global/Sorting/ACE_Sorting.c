//---------------------------------------------------------------------------------------------------
class ACE_Sorting<Class T, Class TContainer, SCR_SortCompare TCompare>
{
	//---------------------------------------------------------------------------------------------------
	static void InsertSort(TContainer a, bool inverse = false)
	{
		for (int i = 1, count = a.Count(); i < count; i++)
		{
			T tmp = a[i];
			
			for (int j = i; j > 0; j--)
			{
				if (!inverse)
				{
					if (!TCompare.Compare(tmp, a[j - 1]))
						break;
				}
				else
				{
					if (!TCompare.Compare(a[j - 1], tmp))
						break;
				}
								
				a.SwapItems(j - 1, j);
			}
		}
	}
}
