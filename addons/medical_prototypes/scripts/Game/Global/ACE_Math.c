//------------------------------------------------------------------------------------------------
class ACE_Math
{
	//------------------------------------------------------------------------------------------------
	static float Exp(float x)
	{
		return Math.Pow(Math.E, x);
	}
	
	//------------------------------------------------------------------------------------------------
	static float Hill(float x)
	{
		return x / (1 + x);
	}
	
	//------------------------------------------------------------------------------------------------
	static float AbsHill(float x)
	{
		return x / (1 + Math.AbsFloat(x));
	}
	
	static bool IsClose(float a, float b, float rtol = 1e5, float atol = 1e-8)
	{
		return Math.AbsFloat(a - b) <= atol + rtol * Math.AbsFloat(b);
	}
}
