//------------------------------------------------------------------------------------------------
class ACE_HexTools
{
	protected static int s_iAscii0 = "0".ToAscii();
	protected static int s_iAscii9 = "9".ToAscii();
	protected static int s_iAsciiA = "A".ToAscii();
	protected static int s_iAsciiF = "F".ToAscii();
	protected static int s_iAsciiLoA = "a".ToAscii();
	protected static int s_iAsciiLoF = "f".ToAscii();
	
	//------------------------------------------------------------------------------------------------
	//! Gets int values of hexadecimal string. Expected to be prefixed with "0x"
	//! Ordered by most significant bits
	static array<int> HexStringToInt(string hexString)
	{
		if (hexString.Substring(0, 2) !=  "0x")
			return {};
		
		int numHexChars = hexString.Length() - 2;
		if (!float.AlmostEqual(Math.Mod(numHexChars, 8), 0))
			return {};
		
		hexString = hexString.Substring(2, numHexChars);

		int numBit32 = (hexString.Length()) / 8;
		array<int> result = {};
		result.Reserve(numBit32);
		
		for (int i_bit32 = 0; i_bit32 < numBit32; i_bit32++)
		{
			int bit32;
			
			for (int i_bit4 = 8 * i_bit32; i_bit4 < 8 * (i_bit32 + 1); i_bit4++)
			{
				int bit4 = HexCharToInt(hexString, index: i_bit4);
				if (bit4 < 0)
					return result;
				
				bit32 = 16 * bit32 + bit4;
			}
			
			result.Insert(bit32);
		}
		
		return result;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Gets int value of a character in a hexadecimal string
	//! Returns -1 if parsing failed
	//! \param index Index of the character, 0 by default.
	static int HexCharToInt(string hexString, int index = 0)
	{
        int value = hexString.ToAscii(index);
		if (value >= s_iAscii0 && value <= s_iAscii9)
			value -= s_iAscii0;
		else if (value >= s_iAsciiA && value <= s_iAsciiF)
			value -= s_iAsciiA - 10;
		else if (value >= s_iAsciiLoA && value <= s_iAsciiLoF)
			value -= s_iAsciiLoA - 10;
		else
			return -1;
		
	    return value;
	}
}
