//------------------------------------------------------------------------------------------------
//! Collection of numerical tools
class ACE_MathTools
{
	//------------------------------------------------------------------------------------------------
	//! Compute root of scalar function f(x) with bisection method
	//! Convergence is met when either xtol or ftol is converged
	//! \param[in] fn scalar function f
	//! \param[in] x0 first starting point
	//! \param[in] x1 second starting point
	//! \param[in] xtol maximum absolute step in x for convergence
	//! \param[in] ftol minimum absolute value of f for convergence
	//! \param[in] maxIter maximum number of iterations
	//! \return result object with solution
	static ACE_MathTools_RootResult<float> Bisect(ACE_MathTools_FunctionBase<float, float> fn, float x0, float x1, float xtol = 1.48e-8, float ftol = -1.0, int maxIter = 100)
	{
		ACE_MathTools_RootResult<float> result = new ACE_MathTools_RootResult<float>();
		float f0 = fn.Eval(x0);
		float f1 = fn.Eval(x1);
		float x2 = 0.5 * (x0 + x1);
		
		if (Math.AbsFloat(f0) <= ftol)
		{
			result.m_fRoot = x0;
			result.m_bConverged = true;
			return result;
		}
		
		if (Math.AbsFloat(f1) <= ftol)
		{
			result.m_fRoot = x1;
			result.m_bConverged = true;
			return result;
		}
		
		if (f0.Sign() == f1.Sign())
		{
			result.m_fRoot = x2;
			result.m_bConverged = false;
			return result;
		}
		
		for (int i = 1; i <= maxIter; ++i)
		{
			float f2 = fn.Eval(x2);
			float dx = 0.5 * (x1 - x0);
			
			if ((Math.AbsFloat(dx) <= xtol) || (Math.AbsFloat(f2) <= ftol))
			{
				result.m_bConverged = true;
				break;
			}
			
			int newSign = f2.Sign();
			
			if (newSign != f0.Sign())
			{
				x1 = x2;
				f1 = f2;
			}
			else if (newSign != f1.Sign())
			{
				x0 = x2;
				f0 = f2;
			}
			else
			{
				result.m_bConverged = false;
				break;
			}
			
			x2 = 0.5 * (x0 + x1);
		}
		
		result.m_fRoot = x2;
		return result;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Compute root of scalar function f(x) with secant method
	//! Convergence is met when either xtol or ftol is converged
	//! \param[in] fn scalar function f
	//! \param[in] x0 first starting point
	//! \param[in] x1 second starting point
	//! \param[in] xtol maximum absolute step in x for convergence
	//! \param[in] ftol minimum absolute value of f for convergence
	//! \param[in] maxIter maximum number of iterations
	//! \return result object with solution
	static ACE_MathTools_RootResult<float> Secant(ACE_MathTools_FunctionBase<float, float> fn, float x0, float x1, float xtol = 1.48e-8, float ftol = -1.0, int maxIter = 50)
	{
		ACE_MathTools_RootResult<float> result = new ACE_MathTools_RootResult<float>();
		float f0 = fn.Eval(x0);
		float f1 = fn.Eval(x1);
		
		if (Math.AbsFloat(f0) <= ftol)
		{
			result.m_fRoot = x0;
			result.m_bConverged = true;
			return result;
		}
		
		if (Math.AbsFloat(f1) <= ftol)
		{
			result.m_fRoot = x1;
			result.m_bConverged = true;
			return result;
		}
		
		for (int i = 1; i <= maxIter; ++i)
		{
			if (f0 == f1)
			{
				result.m_bConverged = false;
				break;
			}
			
			float dx = -f1 * (x1 - x0) / (f1 - f0);
			x0 = x1;
			x1 += dx;
			f0 = f1;
			f1 = fn.Eval(x1);
			
			if ((Math.AbsFloat(dx) <= xtol) || (Math.AbsFloat(f1) <= ftol))
			{
				result.m_bConverged = true;
				break;
			}
		}
		
		result.m_fRoot = x1;
		return result;
	}
}

//------------------------------------------------------------------------------------------------
//! Derive from this class for passing functions to methods in ACE_MathTools
class ACE_MathTools_FunctionBase<Class TX, Class TY> : Managed
{
	//------------------------------------------------------------------------------------------------
	//! Evaluate function value at x
	TY Eval(TX x);
}

//------------------------------------------------------------------------------------------------
//! Result of a root optimization
class ACE_MathTools_RootResult<Class TX> : Managed
{
	TX m_fRoot;
	bool m_bConverged;
}
