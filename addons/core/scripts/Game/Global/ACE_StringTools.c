//------------------------------------------------------------------------------------------------
//! Tools for manipulating strings
class ACE_StringTools
{
	//------------------------------------------------------------------------------------------------
	//! Pad a numeric string with leading zeros to obtain a string of given length
	//! \param[in] str is the numeric string to be padded
	//! \param[in] length
	//! \return the padded string
	static string Zfill(string str, int length)
	{
		string padding;
		
		int n = length - str.Length();
		for (int i = 0; i < n; i++)
			padding += "0";
		
		return padding + str;
	}
}