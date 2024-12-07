//------------------------------------------------------------------------------------------------
class ACE_EntityIdHelper
{
	//------------------------------------------------------------------------------------------------
	static array<int> ToInt(EntityID id)
	{
		return ACE_HexTools.HexStringToInt(ToString(id));
	}
	
	//------------------------------------------------------------------------------------------------
	static EntityID FromString(string str)
	{
		array<int> bits = ACE_HexTools.HexStringToInt(str);
		if (bits.Count() < 2)
			return EntityID.INVALID;

		return EntityID.FromInt(bits[0], bits[1]);
	}
	
	//------------------------------------------------------------------------------------------------
	static string ToString(EntityID id)
	{
		// Drop the last three characters, which are " {}"
		return id.ToString().Substring(0, 18);
	}
}
