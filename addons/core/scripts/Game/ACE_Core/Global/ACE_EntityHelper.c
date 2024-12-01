//------------------------------------------------------------------------------------------------
class ACE_EntityHelper
{
	//------------------------------------------------------------------------------------------------
	static EntityID EntityIdFromString(string str)
	{
		if (str.Length() != 18 || str.Substring(0, 2) !=  "0x")
			return EntityID.INVALID;
		
		int hiBits, loBits;
		if (!ACE_Math.HexStringToInt(str.Substring(2, 8), hiBits) ||
			!ACE_Math.HexStringToInt(str.Substring(10, 8), loBits)
		)
			return EntityID.INVALID;
		
		return EntityID.FromInt(hiBits, loBits);
	}
	
	//------------------------------------------------------------------------------------------------
	static string EntityIdToString(EntityID id)
	{
		return id.ToString().Substring(0, 18);
	}
}
