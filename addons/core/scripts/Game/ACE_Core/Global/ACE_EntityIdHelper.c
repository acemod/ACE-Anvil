//------------------------------------------------------------------------------------------------
class ACE_EntityIdHelper
{
	//------------------------------------------------------------------------------------------------
	static array<int> ToInt(EntityID id)
	{
		array<int> bits = ACE_HexTools.HexStringToInt(ToString(id));
		if (bits.Count() == 1)
			bits.InsertAt(0, 0);

		return bits;
	}

	//------------------------------------------------------------------------------------------------
	static EntityID FromString(string str)
	{
		array<int> bits = ACE_HexTools.HexStringToInt(str);
		if (bits.Count() == 1)
			bits.InsertAt(0, 0);
		else
			if (bits.Count() > 2)
				return EntityID.INVALID;

		return EntityID.FromInt(bits[0], bits[1]);
	}

	//------------------------------------------------------------------------------------------------
	static string ToString(EntityID id)
	{
		// Drop the last three characters, which are " {}"
		string str = id.ToString();

		// Low id items such as trees/bushes end up with this odd format, fix it
		if (str.StartsWith("0x0x"))
			return str.Substring(2, str.Length() - 5);
		else
			return str.Substring(0, str.Length() - 3);
	}
}
