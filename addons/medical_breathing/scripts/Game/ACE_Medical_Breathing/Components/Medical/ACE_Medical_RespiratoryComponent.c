//------------------------------------------------------------------------------------------------
class ACE_Medical_RespiratoryComponentClass : ACE_Medical_BaseComponent3Class
{
}

//------------------------------------------------------------------------------------------------
//! Updates to vitals are mostly server side right now
//! Clients can request values for vitals via ACE_Medical_NetworkComponent
//! TO DO: Inherit from ACE_Medical_BaseComponent once systems support inheritance
class ACE_Medical_RespiratoryComponent : ACE_Medical_BaseComponent3
{
	protected float m_fTargetVentilationMLPM;
	protected float m_fVentilationMLPM;
	protected float m_fRespiratoryRatePM;
	protected float m_fAnaerobicPressure;
	protected float m_fPH;
	protected float m_fPaO2KPA;
	protected float m_fPaCO2KPA;
	protected float m_fHemoglobinKd;
	
	[RplProp()]
	protected bool m_bAreAirwaysBlocked = false;
	[RplProp()]
	protected float m_fPneumothoraxVolumeML = 0;
	[RplProp()]
	protected bool m_bHasTensionPneumothorax = false;
	[RplProp()]
	protected bool m_bHasHemothorax = false;
	
	protected ACE_Medical_RespiratorySystemSettings m_Settings;
	
	//------------------------------------------------------------------------------------------------
	//! Register this component at ACE_Medical_RespiratorySystem
	override typename GetAssociatedSystemType()
	{
		return ACE_Medical_RespiratorySystem;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (settings)
			m_Settings = settings.m_RespiratorySystem;	
	}
	
	//------------------------------------------------------------------------------------------------
	//! Sets target ventilation in ml/min
	void SetTargetVentilation(float vent)
	{
		m_fTargetVentilationMLPM = vent;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns target ventilation in ml/min
	float GetTargetVentilation()
	{
		return m_fTargetVentilationMLPM;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns current ventilation in ml/min
	float GetVentilation()
	{
		return m_fVentilationMLPM;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Sets current ventilation in ml/min
	void SetVentilation(float vent)
	{
		m_fVentilationMLPM = vent;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Sets respiratory rate in 1/min
	void SetRespiratoryRate(float rate)
	{
		m_fRespiratoryRatePM = rate;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns respiratory rate in 1/min
	float GetRespiratoryRate()
	{
		return m_fRespiratoryRatePM;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns anaerobic pressure
	float GetAnaerobicPressure()
	{
		return m_fAnaerobicPressure;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Sets anaerobic pressure
	void SetAnaerobicPressure(float pressure)
	{
		m_fAnaerobicPressure = pressure;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns blood pH
	float GetPH()
	{
		return m_fPH;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Sets blood pH
	void SetPH(float pH)
	{
		m_fPH = pH;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Sets partial pressure of CO2 in kPa
	void SetPaCO2(float pressure)
	{
		m_fPaCO2KPA = pressure;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns partial pressure of CO2 in kPa
	float GetPaCO2()
	{
		return m_fPaCO2KPA;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Sets partial pressure of O2 in kPa
	void SetPaO2(float pressure)
	{
		m_fPaO2KPA = pressure;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns partial pressure of O2 in kPa
	float GetPaO2()
	{
		return m_fPaO2KPA;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Sets O2 dissociation constant of hemoglobin
	void SetHemoglobinKd(float kd)
	{
		m_fHemoglobinKd = kd;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns O2 dissociation constant of hemoglobin
	float GetHemoglobinKd()
	{
		return m_fHemoglobinKd;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetAreAirwayBlocked(bool areBlocked)
	{
		m_bAreAirwaysBlocked = areBlocked;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool AreAirwaysBlocked()
	{
		return m_bAreAirwaysBlocked;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetPneumothoraxVolume(float volume)
	{
		m_fPneumothoraxVolumeML = volume;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	float GetPneumothoraxVolume()
	{
		return m_fPneumothoraxVolumeML;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetHasTensionPneumothorax(bool hasTPTX)
	{
		m_bHasTensionPneumothorax = hasTPTX;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool HasTensionPneumothorax()
	{
		return m_bHasTensionPneumothorax;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetHasHemothorax(bool hasHPTX)
	{
		m_bHasHemothorax = hasHPTX;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool HasHemothorax()
	{
		return m_bHasHemothorax;
	}
}
