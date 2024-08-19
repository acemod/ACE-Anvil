//------------------------------------------------------------------------------------------------
class ACE_Medical_Dose : ScriptAndConfig
{
	[Attribute(desc: "Administered concentration [nM]")]
	protected float m_fConcentrationNM;
	
	protected int m_iAdministrationTimeMS;
	
	//------------------------------------------------------------------------------------------------
	//! Sets administration time to current time
	void SetAdministrationTime()
	{
		m_iAdministrationTimeMS = System.GetTickCount();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Administered concentration [nM]
	float GetConcentration()
	{
		return m_fConcentrationNM;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Time passed since dose was administered [s]
	float GetElapsedTime()
	{
		return System.GetTickCount(m_iAdministrationTimeMS) / 1000;
	}
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_Bolus : ACE_Medical_Dose
{
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_Infusion : ACE_Medical_Dose
{
	[Attribute(desc: "Duration of infusion [s]")]
	protected float m_fInfusionDurationS;
	
	//------------------------------------------------------------------------------------------------
	//! Infusion duraction in seconds
	float GetInfusionDuration()
	{
		return m_fInfusionDurationS;
	}
}
