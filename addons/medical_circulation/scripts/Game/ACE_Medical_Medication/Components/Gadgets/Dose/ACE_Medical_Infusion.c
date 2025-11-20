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
	override float ComputeConcentration(ACE_Medical_PharmacokineticsConfig config, ACE_Medical_CharacterContext context = null)
	{
		float time = GetElapsedTime();
		if (time > m_fInfusionDurationS)
			m_bIsStopped = true;
		
		float bloodFlowScale = 1;
		if (context && context.m_pDamageManager)
			bloodFlowScale = context.m_pDamageManager.ACE_Medical_GetBloodFlowScale();
		
		float effectiveRate = m_fInfusionRate * bloodFlowScale;
		float concentration = effectiveRate / config.m_fDeactivationRateConstant;
		
		if (m_bIsStopped)
			concentration *= ACE_Math.Exp(-config.m_fDeactivationRateConstant * time);
		else
			concentration *= 1 - ACE_Math.Exp(-config.m_fDeactivationRateConstant * time);
		
		if (time > m_fInfusionDurationS)
			m_bIsExpired = concentration < EXPIRED_DOSE_PERCENTAGE * effectiveRate / config.m_fDeactivationRateConstant;

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
	
	//------------------------------------------------------------------------------------------------
	override ACE_Medical_Dose CloneDose()
	{
		ACE_Medical_Infusion copy = ACE_Medical_Infusion.Cast(super.CloneDose());
		if (!copy)
			return null;
		
		copy.m_fInfusionRate = m_fInfusionRate;
		copy.m_fInfusionDurationS = m_fInfusionDurationS;
		copy.m_bIsStopped = false;
		return copy;
	}
}
