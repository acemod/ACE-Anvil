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
	
	//------------------------------------------------------------------------------------------------
	//! Does bilinear interpolation between `a`, `b`, `c` and `d` using `tx` and `ty`.
	//! \param[in] a Value at (0, 0)
	//! \param[in] b Value at (1, 0)
	//! \param[in] c Value at (0, 1)
	//! \param[in] d Value at (1, 1)
	//! \return interpolated value
	float Bilerp(float a, float b, float c, float d, float tx, float ty)
	{
		float y0 = Math.Lerp(a, b, tx);
		float y1 = Math.Lerp(c, d, tx);
		return Math.Lerp(y0, y1, ty);
	}
}
