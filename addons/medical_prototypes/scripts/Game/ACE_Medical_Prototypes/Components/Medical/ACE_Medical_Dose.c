//------------------------------------------------------------------------------------------------
class ACE_Medical_Dose : ScriptAndConfig
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, desc: "Type of administered drug", enums: ParamEnumArray.FromEnum(ACE_Medical_EDrugType))]
	protected ACE_Medical_EDrugType m_eDrugType;
	
	protected int m_iAdministrationTimeMS;
	protected bool m_bIsExpired = false;
	
	protected static const float EXPIRED_DOSE_PERCENTAGE = 0.01; // Concentration at which dose is considered expired [percentage of initial]
	
	//------------------------------------------------------------------------------------------------
	//! Computes the current concentration of this dose based on the administration time and the
	//! passed config for the pharamockinetics
	float ComputeConcentration(ACE_Medical_PharmacokineticsConfig config);
	
	//------------------------------------------------------------------------------------------------
	//! Get the type of the drug administered by this dose
	ACE_Medical_EDrugType GetDrugType()
	{
		return m_eDrugType;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Sets administration time to current time
	void SetAdministrationTime()
	{
		m_iAdministrationTimeMS = System.GetTickCount();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Time passed since dose was administered [s]
	float GetElapsedTime()
	{
		return System.GetTickCount(m_iAdministrationTimeMS) / 1000;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Return true when the dose's concentration has dropped so low that it should be considered expired
	bool IsExpired()
	{
		return m_bIsExpired;
	}
}

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

//------------------------------------------------------------------------------------------------
//! Drug should follow the kinetics of a IV infusion
class ACE_Medical_Infusion : ACE_Medical_Dose
{
	[Attribute(desc: "Infusion rate [nM/s]")]
	protected float m_fInfusionRate;
	
	[Attribute(desc: "Duration of infusion [s]")]
	protected float m_fInfusionDurationS;
	
	//------------------------------------------------------------------------------------------------
	override float ComputeConcentration(ACE_Medical_PharmacokineticsConfig config)
	{
		float time = GetElapsedTime();
		float concentration = m_fInfusionRate / config.m_fDeactivationRateConstant;
		
		if (time <= m_fInfusionDurationS)
			concentration *= 1 - ACE_Math.Exp(-config.m_fDeactivationRateConstant * time);
		else
			concentration *= ACE_Math.Exp(-config.m_fDeactivationRateConstant * time);
		
		if (time > m_fInfusionDurationS)
			m_bIsExpired = concentration < EXPIRED_DOSE_PERCENTAGE * m_fInfusionRate / config.m_fDeactivationRateConstant;

		return concentration;
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
