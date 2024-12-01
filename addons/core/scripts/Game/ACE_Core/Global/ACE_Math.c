//------------------------------------------------------------------------------------------------
class ACE_Math
{
	protected static int s_iAscii0 = "0".ToAscii();
	protected static int s_iAscii9 = "9".ToAscii();
	protected static int s_iAsciiA = "A".ToAscii();
	protected static int s_iAsciiF = "F".ToAscii();
	protected static int s_iAsciiLoA = "a".ToAscii();
	protected static int s_iAsciiLoF = "f".ToAscii();
	
	//------------------------------------------------------------------------------------------------
	static bool HexStringToInt(string hexString, out int result)
	{
	    for (int i = 0; i < hexString.Length() ; i++)
	    {
	        int value = hexString.ToAscii(i);
			if (value >= s_iAscii0 && value <= s_iAscii9)
				value -= s_iAscii0;
			else if (value >= s_iAsciiA && value <= s_iAsciiF)
				value -= s_iAsciiA - 10;
			else if (value >= s_iAsciiLoA && value <= s_iAsciiLoF)
				value -= s_iAsciiLoA - 10;
			else
				return false;

	        result = 16 * result + value;
	    }
	
	    return true;
	}
}
