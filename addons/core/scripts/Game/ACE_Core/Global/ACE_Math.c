//------------------------------------------------------------------------------------------------
class ACE_Math
{
	//------------------------------------------------------------------------------------------------
	//! Return Euler's constant raised to the power of x
	static float Exp(float x)
	{
		return Math.Pow(Math.E, x);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Evaluate normalized Hill equation at x
	static float Hill(float x)
	{
		return x / (1 + x);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Round float to a given precision in decimal digits
	//! \param[in] value value to be rounded
	//! \param[in] precision number of decimal places to round to
	//! \return rounded float
	static float Round(float value, int precision)
	{
		float scale = Math.Pow(10, precision);
		return Math.Round(value * scale) / scale;
	}
}
