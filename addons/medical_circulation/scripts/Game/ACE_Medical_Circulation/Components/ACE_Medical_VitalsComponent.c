//------------------------------------------------------------------------------------------------
class ACE_Medical_VitalsComponentClass : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Updates to vitals are mostly server side right now
//! Clients can request values for vitals via ACE_Medical_NetworkComponent
class ACE_Medical_VitalsComponent : ScriptComponent
{
	[RplProp()]
	protected ACE_Medical_EVitalStateID m_eVitalStateID = ACE_Medical_EVitalStateID.STABLE;
	
	[RplProp()]
	protected bool m_bIsCPRPerformed = false;
	
	protected bool m_bWasRevived = false;
	
	protected float m_fHeartRateBPM;
	protected float m_fCardiacOutput;
	protected float m_fSystemicVascularResistance;
	protected float m_fMeanArterialPressureKPA;
	protected float m_fPulsePressureKPA;
	
	protected float m_fHeartRateMedicationAdjustment = 0;
	protected float m_fSystemicVascularResistanceMedicationAdjustment = 0;
	protected float m_fReviveSuccessCheckTimerScale = 1;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
	
		if (!GetGame().InPlayMode() || !Replication.IsServer())
			return;
		
		Reset();
	
		ACE_Medical_VitalStatesSystem system = ACE_Medical_VitalStatesSystem.GetInstance(owner.GetWorld());
		if (system)
			system.Register(SCR_ChimeraCharacter.Cast(owner));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Friend method of ACE_Medical_IVitalState
	void SetVitalStateID(ACE_Medical_EVitalStateID newStateID)
	{
		if (m_eVitalStateID == newStateID)
			return;
		
		m_eVitalStateID = newStateID;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns vital state. Can also be called by clients.
	ACE_Medical_EVitalStateID GetVitalStateID()
	{
		return m_eVitalStateID;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetIsCPRPerformed(bool isPerformed)
	{
		m_bIsCPRPerformed = isPerformed;
		Replication.BumpMe();
	}
	//------------------------------------------------------------------------------------------------
	//! Returns true when in someone is peforming CPR on this system. Can also be called by clients.
	bool IsCPRPerformed()
	{
		return m_bIsCPRPerformed;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnRevived()
	{
		m_bWasRevived = true;
	}
	
	//------------------------------------------------------------------------------------------------
	void ClearReviveHistory()
	{
		m_bWasRevived = false;
	}
	
	//------------------------------------------------------------------------------------------------
	bool WasRevived()
	{
		return m_bWasRevived;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetHeartRate(float heartRate)
	{
		m_fHeartRateBPM = heartRate;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns heart rate in bpm
	float GetHeartRate()
	{
		return m_fHeartRateBPM;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Set cardiac output in ml/min
	void SetCardiacOutput(float cardiacOutput)
	{
		m_fCardiacOutput = cardiacOutput;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns cardiac output in ml/min
	float GetCardiacOutput()
	{
		return m_fCardiacOutput;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Set systemic vascular resistence in kPa * min / ml
	void SetSystemicVascularResistance(float resistance)
	{
		m_fSystemicVascularResistance = resistance;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns systemic vascular resistence in kPa * min / ml
	float GetSystemicVascularResistance()
	{
		return m_fSystemicVascularResistance;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Set mean arterial pressure in kPA
	void SetMeanArterialPressure(float pressure)
	{
		m_fMeanArterialPressureKPA = pressure;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns mean arterial pressure in kPA
	float GetMeanArterialPressure()
	{
		return m_fMeanArterialPressureKPA;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Set pulse pressure in kPA
	void SetPulsePressure(float pressure)
	{
		m_fPulsePressureKPA = pressure;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns pulse pressure in kPA
	float GetPulsePressure()
	{
		return m_fPulsePressureKPA;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns diastolic and systolic blood pressures in kPa
	Tuple2<float, float> GetBloodPressures()
	{
		return new Tuple2<float, float>
		(
			m_fMeanArterialPressureKPA - 1/3 * m_fPulsePressureKPA,
			m_fMeanArterialPressureKPA + 2/3 * m_fPulsePressureKPA
		);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetHeartRateMedicationAdjustment(float adjustment)
	{
		m_fHeartRateMedicationAdjustment = adjustment;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetHeartRateMedicationAdjustment()
	{
		return m_fHeartRateMedicationAdjustment;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetSystemicVascularResistenceMedicationAdjustment(float adjustment)
	{
		m_fSystemicVascularResistanceMedicationAdjustment = adjustment;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetSystemicVascularResistenceMedicationAdjustment()
	{
		return m_fSystemicVascularResistanceMedicationAdjustment;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Set scale for accelerating the revive success check timer
	void SetReviveSuccessCheckTimerScale(float scale)
	{
		m_fReviveSuccessCheckTimerScale = scale;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetReviveSuccessCheckTimerScale()
	{
		return m_fReviveSuccessCheckTimerScale;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Resets vitals to defaults
	void Reset()
	{
		ACE_Medical_Circulation_Settings settings = ACE_SettingsHelperT<ACE_Medical_Circulation_Settings>.GetModSettings();
		if (settings)
			SetHeartRate(settings.m_fDefaultHeartRateBPM);
		
		SetVitalStateID(ACE_Medical_EVitalStateID.STABLE);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnDelete(IEntity owner)
	{
		super.OnDelete(owner);
	
		if (!GetGame().InPlayMode() || !Replication.IsServer())
			return;
	
		ACE_Medical_VitalStatesSystem system = ACE_Medical_VitalStatesSystem.GetInstance(owner.GetWorld());
		if (system)
			system.Unregister(SCR_ChimeraCharacter.Cast(owner));
	}
}
