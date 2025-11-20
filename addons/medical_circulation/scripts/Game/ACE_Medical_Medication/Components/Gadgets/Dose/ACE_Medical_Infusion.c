//------------------------------------------------------------------------------------------------
//! Drug should follow the kinetics of a IV infusion
class ACE_Medical_Infusion : ACE_Medical_Dose
{
	[Attribute(desc: "Infusion rate [nM/s]")]
	protected float m_fInfusionRate;
	
	[Attribute(desc: "Duration of infusion [s]")]
	protected float m_fInfusionDurationS;
	
	protected bool m_bIsStopped = false;
	
	//------------------------------------------------------------------------------------------------
	override float ComputeConcentration(ACE_Medical_PharmacokineticsConfig config)
	{
		float time = GetElapsedTime();
		if (time > m_fInfusionDurationS)
			m_bIsStopped = true;
		
		float concentration = m_fInfusionRate / config.m_fDeactivationRateConstant;
		
		if (m_bIsStopped)
			concentration *= ACE_Math.Exp(-config.m_fDeactivationRateConstant * time);
		else
			concentration *= 1 - ACE_Math.Exp(-config.m_fDeactivationRateConstant * time);
		
		if (time > m_fInfusionDurationS)
			m_bIsExpired = concentration < EXPIRED_DOSE_PERCENTAGE * m_fInfusionRate / config.m_fDeactivationRateConstant;

		return concentration;
	}
	
	//------------------------------------------------------------------------------------------------
	void Stop()
	{
		m_bIsStopped = true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Infusion duraction in nM/s
	float GetInfusionRate()
	{
		return m_fInfusionRate;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Infusion duraction in seconds
	float GetInfusionDuration()
	{
		return m_fInfusionDurationS;
	}
}
