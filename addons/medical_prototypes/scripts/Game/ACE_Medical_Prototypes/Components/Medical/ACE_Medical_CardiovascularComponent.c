//------------------------------------------------------------------------------------------------
class ACE_Medical_CardiovascularComponentClass : ACE_Medical_BaseComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Updates to vitals are mostly server side right now
//! Clients can request values for vitals via ACE_Medical_NetworkComponent
class ACE_Medical_CardiovascularComponent : ACE_Medical_BaseComponent
{	
	[RplProp()]
	protected ACE_Medical_EVitalState m_eVitalState = ACE_Medical_EVitalState.STABLE;
	[RplProp()]
	protected bool m_bIsCPRPerformed = false;
	protected bool m_bWasInCardiacArrest = false;
	
	// TO DO: Think about whether we want to replicate vitals and how often
	protected float m_fHeartRateBPM;
	protected float m_fCardiacOutput;
	protected float m_fSystemicVascularResistance;
	protected float m_fMeanArterialPressureKPA;
	protected float m_fPulsePressureKPA;
	protected float m_fResilienceRecoveryScale = 1;
	protected float m_fReviveSuccessCheckTimerScale = 1;
	
	protected float m_fHeartRateMedicationAdjustment = 0;
	protected float m_fSystemicVascularResistanceMedicationAdjustment = 0;
	
	protected SCR_CharacterDamageManagerComponent m_pDamageManager;
	protected ACE_Medical_CardiovascularSystemSettings m_Settings;
	
	protected ref ScriptInvokerInt2 m_OnVitalStateChanged = new ScriptInvokerInt2();
	
	static const float KPA2MMHG = 7.50062;
	
	//------------------------------------------------------------------------------------------------
	//! Register this component at ACE_Medical_CardiovascularSystem
	override typename GetAssociatedSystemType()
	{
		return ACE_Medical_CardiovascularSystem;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (settings)
			m_Settings = settings.m_CardiovascularSystem;
				
		m_pDamageManager = SCR_CharacterDamageManagerComponent.Cast(owner.FindComponent(SCR_CharacterDamageManagerComponent));
		
		SCR_CharacterControllerComponent characterController = SCR_CharacterControllerComponent.Cast(owner.FindComponent(SCR_CharacterControllerComponent));
		if (characterController)
			characterController.m_OnLifeStateChanged.Insert(OnLifeStateChanged);		
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnVitalStateChanged(ACE_Medical_EVitalState newState, ACE_Medical_EVitalState prevState)
	{
		// Kill AI that enters cardiac arrest
		if (IsInCardiacArrest() && !SCR_EntityHelper.IsAPlayer(GetOwner()) && !m_Settings.m_bCardiacArrestForAIEnabled)
			m_pDamageManager.Kill(m_pDamageManager.GetInstigator());
		
		// Resilience falls to zero and cannot recover while vitals are at critical vital state
		if (m_eVitalState >= ACE_Medical_EVitalState.CRITICAL)
		{
			m_pDamageManager.GetResilienceHitZone().SetHealth(0);
			m_pDamageManager.UpdateConsciousness();
		}
		
		if (newState == ACE_Medical_EVitalState.CARDIAC_ARREST)
			OnCardiacArrestStateChanged(true);
		else if (prevState == ACE_Medical_EVitalState.CARDIAC_ARREST)
			OnCardiacArrestStateChanged(false);
			
		m_OnVitalStateChanged.Invoke(newState, prevState);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnCardiacArrestStateChanged(bool inCardiacArrest)
	{
		// Add/remove damage effect for cardiac arrest state
		if (inCardiacArrest)
		{
			m_pDamageManager.AddDamageEffect(m_Settings.m_CardiacArrestDamageEffect);
			m_bWasInCardiacArrest = true;
		}
		else
		{
			m_pDamageManager.TerminateDamageEffectsOfType(m_Settings.m_CardiacArrestDamageEffect.Type());
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Reset m_bWasInCardiacArrest when gaining consciousness 
	protected void OnLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState)
	{
		if ((previousLifeState == ECharacterLifeState.INCAPACITATED) && (newLifeState == ECharacterLifeState.ALIVE))
			m_bWasInCardiacArrest = false;
	}
	
	//------------------------------------------------------------------------------------------------
	void Revive()
	{
		SetHeartRate(m_Settings.m_CriticalThresholds.m_fHeartRateLowBPM);
		SetVitalState(ACE_Medical_EVitalState.CRITICAL);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetVitalState(ACE_Medical_EVitalState newState)
	{
		if (m_eVitalState == newState)
			return;
		
		ACE_Medical_EVitalState prevState = m_eVitalState;
		Replication.BumpMe();
		m_eVitalState = newState;
		OnVitalStateChanged(newState, prevState);
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_EVitalState GetVitalState()
	{
		return m_eVitalState;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns true when in cardiac arrest. Can also be called by clients.
	bool IsInCardiacArrest()
	{
		return m_eVitalState == ACE_Medical_EVitalState.CARDIAC_ARREST;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns true when owner was in cardiac arrest before (gets reset when becoming conscious)
	bool WasInCardiacArrest()
	{
		return m_bWasInCardiacArrest;
	}
	
	
	//------------------------------------------------------------------------------------------------
	void SetIsCPRPerformed(bool isPerformed)
	{
		m_bIsCPRPerformed = isPerformed;
		Replication.BumpMe();
		OnCPRPerformedChanged(isPerformed);
	}
	//------------------------------------------------------------------------------------------------
	//! Returns true when in someone is peforming CPR on this system. Can also be called by clients.
	bool IsCPRPerformed()
	{
		return m_bIsCPRPerformed;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnCPRPerformedChanged(bool isPerformed)
	{
		if (!IsInCardiacArrest())
			return;
		
		if (isPerformed)
			m_pDamageManager.TerminateDamageEffectsOfType(m_Settings.m_CardiacArrestDamageEffect.Type());
		else
			m_pDamageManager.AddDamageEffect(m_Settings.m_CardiacArrestDamageEffect);
		
	}
	
	//------------------------------------------------------------------------------------------------
	//! Set resilience recovery scale based on vitals
	void SetResilienceRecoveryScale(float scale)
	{
		m_fResilienceRecoveryScale = scale;
	}
	
	
	//------------------------------------------------------------------------------------------------
	//! Resilience recovery scale based on vitals
	float GetResilienceRecoveryScale()
	{
		return m_fResilienceRecoveryScale;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetHeartRate(float heartRate)
	{
		m_fHeartRateBPM = heartRate;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns heart rate in BPM
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
	ScriptInvokerInt2 GetOnVitalStateChanged()
	{
		return m_OnVitalStateChanged;
	}
}
