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
		int numNibbles = hexString.Length() - 2;
		// Calculate how many 32 bit integers are needed to store the hex
		int numValues  = ((numNibbles + 7) & ~7) / 8;
		result.Reserve(numValues);
		
		int value = 0;
		int outIdx = 0;
		
		for (int inpIdx = numNibbles - 1; inpIdx >= 0; --inpIdx)
		{
			int nibble = hexString.ToAscii(inpIdx + 2);
			
			if (nibble >= s_iAscii0 && nibble <= s_iAscii9)
				nibble -= s_iAscii0;
			else if (nibble >= s_iAsciiA && nibble <= s_iAsciiF)
				nibble -= s_iAsciiA - 10;
			else
				return {}; //invalid character in string
			
			// Add the nibble to the correct position in the 32 bit integer
			value |= nibble << 4 * outIdx;
			
			if (++outIdx == 8 || inpIdx == 0)
			{
				result.InsertAt(value, --numValues);
				value = 0;
				outIdx = 0;
			}		
		}
		
		return result;
	}
}
