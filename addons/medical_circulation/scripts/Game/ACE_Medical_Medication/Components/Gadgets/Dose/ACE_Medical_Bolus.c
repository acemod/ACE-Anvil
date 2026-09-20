//------------------------------------------------------------------------------------------------
//! Drug should follow the kinetics of a IV bolus
class ACE_Medical_Bolus : ACE_Medical_Dose
{
	// TO DO: Sample concentration from a gaussian distribution for randomization
	[Attribute(desc: "Administered concentration [nM]")]
	protected float m_fConcentrationNM;
	
	//------------------------------------------------------------------------------------------------
	override float ComputeConcentration(ACE_Medical_PharmacokineticsConfig config)
	{
		float time = GetElapsedTime();
		float concentration = config.m_fActivationRateConstant / (config.m_fActivationRateConstant - config.m_fDeactivationRateConstant);
		concentration *= ACE_Math.Exp(-config.m_fDeactivationRateConstant * time) - ACE_Math.Exp(-config.m_fActivationRateConstant * time);
		concentration *= GetAdministeredConcentration();
		
		// 1 / k_a is the mean lifetime of the precursor state, so we do expiration checks after this
		if (time > 1 / config.m_fActivationRateConstant)
			m_bIsExpired = concentration < EXPIRED_DOSE_PERCENTAGE * GetAdministeredConcentration();
		
		return concentration;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Administered concentration [nM]
	float GetAdministeredConcentration()
	{
		return m_fConcentrationNM;
	}
}
