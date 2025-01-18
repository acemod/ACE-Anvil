//------------------------------------------------------------------------------------------------
class ACE_HexTools
{
	protected static int s_iAscii0 = "0".ToAscii();
	protected static int s_iAscii9 = "9".ToAscii();
	protected static int s_iAsciiA = "a".ToAscii();
	protected static int s_iAsciiF = "f".ToAscii();
		
	//------------------------------------------------------------------------------------------------
	//! Gets int values of hexadecimal string. Expected to be prefixed with "0x"
	//! Ordered by most significant bits
	static array<int> HexStringToInt(string hexString)
	{
		hexString.TrimInPlace();
		hexString.ToLower();
		if (!hexString.StartsWith("0x"))
			return {};

		array<int> result = {};
		int nibbles = hexString.Length() - 2;
		int values  = ((nibbles + 7) & ~7) / 8;
		result.Reserve(values);
		
		int byte  = 0;
		int value = 0;
		int count = 0;
		while(nibbles > 0)
		{
			int nibble = hexString.Get(nibbles + 1).ToAscii();
			
			if (nibble >= s_iAscii0 && nibble <= s_iAscii9)
				nibble -= s_iAscii0;
			else
				if (nibble >= s_iAsciiA && nibble <= s_iAsciiF)
					nibble -= s_iAsciiA - 10;
				else
					return {}; //invalid character in string

			value <<= 4;
			value |= nibble;			
			--nibbles;
			
			if (++count == 8 || nibbles == 0)
			{
				int rev = 0;
				for(int i = 0; i < count; ++i)
				{
					rev = (rev << 4) + (value & 0xf);
					value >>= 4;
				}
				
				result.InsertAt(rev, --values);
				value = 0;
				count = 0;
			}			
		}
						
		return result;
	}
}
