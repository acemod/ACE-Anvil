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
		// See: https://feedback.bistudio.com/T188436
		if (str == "0x(nil)" || str == "(nil)")
			return EntityID.INVALID;

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
		if (id == EntityID.INVALID)
			return "0x0";

		// Drop the last three characters, which are " {}"
		string str = id.ToString();

		// Low id items such as trees/bushes end up with this odd format, fix it
		// See: https://feedback.bistudio.com/T188436
		if (str.StartsWith("0x0x"))
			str = str.Substring(2, str.Length() - 5);
		else
			str = str.Substring(0, str.Length() - 3);

		// Just incase EntityID.ToString still returned an invalid value
		if (str == "0x(nil)" || str == "(nil)")
			return "0x0";

		return str;
	}
}
