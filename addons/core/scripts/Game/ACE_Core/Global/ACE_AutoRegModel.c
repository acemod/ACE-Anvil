//------------------------------------------------------------------------------------------------
//! Auto regressive model of order 1
//! Useful for generating auto-correlated noise
class ACE_AutoRegModel : Managed
{
	protected float m_fRho;
	protected float m_fSigmaEps;
	protected float m_fPoint;
	
	//------------------------------------------------------------------------------------------------
	//! \param[in] rho Auto regression coefficient
	//! \param[in] sigma Standard deviation of the noise
	void ACE_AutoRegModel(float rho, float sigma)
	{
		m_fRho = rho;
		m_fSigmaEps = sigma * Math.Sqrt(1.0 - Math.Pow(m_fRho, 2));
		m_fPoint = Math.RandomGaussFloat(sigma, 0.0);
	}
	
	//------------------------------------------------------------------------------------------------
	//! \return next point in series
	float SamplePoint()
	{
		m_fPoint = m_fRho * m_fPoint + Math.RandomGaussFloat(m_fSigmaEps, 0.0);
		return m_fPoint;
	}
}