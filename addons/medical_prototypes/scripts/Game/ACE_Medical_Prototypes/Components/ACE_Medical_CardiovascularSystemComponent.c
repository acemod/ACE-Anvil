//------------------------------------------------------------------------------------------------
class ACE_Medical_CardiovascularSystemComponentClass : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Updates to vitals are mostly server side right now.
//! Clients can request values for vitals via ACE_Medical_NetworkComponent
class ACE_Medical_CardiovascularSystemComponent : ScriptComponent
{
	[Attribute(defvalue: "1", desc: "Timeout between updates of vitals [s]")]
	protected float m_fVitalsUpdateTimeoutS;
	protected float m_fVitalsUpdateTimerS = 0;
	protected float m_fCheckCriticalHeartRateTimeoutS = 5;
	protected float m_fCheckCriticalHeartRateTimerS = 0;
	
	[Attribute(defvalue: "80", desc: "Default heart rate [BPM]", category: "Default Vitals")]
	protected float m_fDefaultHeartRateBPM;
	
	[Attribute(defvalue: "95", desc: "Default stroke volume [ml]", category: "Default Vitals")]
	protected float m_fDefaultStrokeVolumeML;
	
	[Attribute(defvalue: "12.443", desc: "Default mean arterial pressure [kPa]", category: "Default Vitals")]
	protected float m_fDefaultMeanArterialPressureKPA;
	
	[Attribute(defvalue: "5.3329", desc: "Default pulse pressure [kPa]", category: "Default Vitals")]
	protected float m_fDefaultPulsePressureKPA;
	
	[Attribute(defvalue: "40", desc: "A heart rate below this will results in critical state [BPM]", category: "Critical State")]
	protected float m_fCriticalHeartRateThresholdLowBPM;
	
	[Attribute(defvalue: "220", desc: "A heart rate above this will results in critical state [BPM]", category: "Critical State")]
	protected float m_fCriticalHeartRateThresholdHighBPM;
	
	[Attribute(defvalue: "7.11", desc: "An arterial pressure below this will result in critical state [kPa]", category: "Critical State")]
	protected float m_fCriticalMeanArterialPressureThresholdLowKPA;
	
	[Attribute(defvalue: "29.5", desc: "An arterial pressure above this will result in critical state [kPa]", category: "Critical State")]
	protected float m_fCriticalMeanArterialPressureThresholdHighKPA;
	
	[Attribute(defvalue: "CLASS_4_HEMORRHAGE", uiwidget: UIWidgets.SearchComboBox, desc: "A blood level below this will result in critical state", enums: ParamEnumArray.FromEnum(ACE_Medical_EBloodState), category: "Critical State")]
	protected ACE_Medical_EBloodState m_eCriticalBloodLevelThreshold;
	
	[Attribute(desc: "Damage effects caused by cardiac arrest", category: "Cardiac Arrest")]
	protected ref ACE_Medical_CardiacArrestDamageEffect m_CardiacArrestDamageEffect;
	
	[Attribute(defvalue: "20", desc: "A heart rate below this will results in cardiac arrest [BPM]", category: "Cardiac Arrest")]
	protected float m_fCardiacArrestHeartRateThresholdLowBPM;
	
	[Attribute(defvalue: "220", desc: "A heart rate above this will results in cardiac arrest [BPM]", category: "Cardiac Arrest")]
	protected float m_fCardiacArrestHeartRateThresholdHighBPM;
	
	[Attribute(defvalue: "5.21", desc: "An arterial pressure below this will result in cardiac arrest when the heart rate is critical [kPa]", category: "Cardiac Arrest")]
	protected float m_fCardiacArrestMeanArterialPressureThresholdLowKPA;
	
	[Attribute(defvalue: "29.5", desc: "An arterial pressure above this will result in cardiac arrest [kPa]", category: "Cardiac Arrest")]
	protected float m_fCardiacArrestMeanArterialPressureThresholdHighKPA;
	
	[Attribute(defvalue: "CLASS_4_HEMORRHAGE", uiwidget: UIWidgets.SearchComboBox, desc: "A blood level below this will result in cardiac arrest", enums: ParamEnumArray.FromEnum(ACE_Medical_EBloodState), category: "Cardiac Arrest")]
	protected ACE_Medical_EBloodState m_eCardiacArrestBloodLevelThreshold;
	
	[Attribute(defvalue: "15", desc: "Default delay between check for successful CPR [s]", category: "CPR")]
	protected float m_fCPRSuccessCheckTimeoutS;
	protected float m_fCPRSuccessCheckTimerS = 0;
	
	[Attribute(defvalue: "0", desc: "Chance for exiting cardiac arrest while performing CPR at class IV hemorrhage", category: "CPR")]
	protected float m_fCPRSuccessChanceMin;
	
	[Attribute(defvalue: "0.4", desc: "Chance for exiting cardiac arrest while performing CPR at above class II hemorrhage", category: "CPR")]
	protected float m_fCPRSuccessChanceMax;
	
	[Attribute(defvalue: "CLASS_2_HEMORRHAGE", uiwidget: UIWidgets.SearchComboBox, desc: "A blood level below this will prevent resilience from recovering", enums: ParamEnumArray.FromEnum(ACE_Medical_EBloodState), category: "Resilience")]
	protected ACE_Medical_EBloodState m_eMinBloodLevelForResilienceRecovery;
	
	[Attribute(defvalue: "0.2", desc: "Maximum resilience recovery scale applied after getting revived. This scale gets multiplied by the scaled health of the brain and will be removed once the patient becomes conscious", category: "Resilience")]
	protected float m_fMaxRevivalResilienceRecoveryScale;
		
	[RplProp()]
	protected ACE_Medical_EVitalState m_eVitalState = ACE_Medical_EVitalState.NORMAL;
	[RplProp()]
	protected bool m_bIsCPRPerformed = false;
	
	// Default vitals derived from others
	protected float m_fDefaultSystemicVascularResistance;
	protected float m_fPulsePressureScale;
	
	// TO DO: Think about whether we want to replicate vitals and how often
	protected float m_fHeartRateBPM;
	protected float m_fCardiacOutput;
	protected float m_fSystemicVascularResistance;
	protected float m_fMeanArterialPressureKPA;
	protected float m_fPulsePressureKPA;
	
	// Threshold derived from others
	protected float m_fMeanCriticalCardiacArrestHeartRateThresholdLowBPM;
	// Becomes true when entering cardiac arrest and becomes false when becoming conscious
	protected bool m_bWasInCardiacArrest = false;
	
	protected float m_fResilienceRecoveryScale = 1;
	
	protected SCR_CharacterDamageManagerComponent m_pDamageManager;
	protected SCR_CharacterBloodHitZone m_pBloodHZ;
	protected ACE_Medical_BrainHitZone m_pBrainHZ;
	protected SCR_CharacterControllerComponent m_pCharacterController;
	
	protected ref ScriptInvokerInt2 m_OnVitalStateChanged = new ScriptInvokerInt2();
	
	static const float KPA2MMHG = 7.50062;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
		
		SetEventMask(owner, EntityEvent.INIT);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
		m_pDamageManager = SCR_CharacterDamageManagerComponent.Cast(owner.FindComponent(SCR_CharacterDamageManagerComponent));	
		m_pBloodHZ = m_pDamageManager.GetBloodHitZone();
		m_pBrainHZ = m_pDamageManager.ACE_Medical_GetBrainHitZone();
		m_pCharacterController = SCR_CharacterControllerComponent.Cast(owner.FindComponent(SCR_CharacterControllerComponent));
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (settings)
		{
			m_fCPRSuccessCheckTimeoutS = settings.m_fCPRSuccessCheckTimeoutS;
			m_fCPRSuccessChanceMin = settings.m_fCPRSuccessChanceMin;
			m_fCPRSuccessChanceMax = settings.m_fCPRSuccessChanceMax;
		}
		
		// Derive default SVR from other values
		m_fDefaultSystemicVascularResistance = m_fDefaultMeanArterialPressureKPA / (m_fDefaultHeartRateBPM * m_fDefaultStrokeVolumeML);
		// MAP multiplied by this value yields PP
		m_fPulsePressureScale = m_fDefaultPulsePressureKPA / m_fDefaultMeanArterialPressureKPA;
		// Mean between critical and cardiac arrest heart rate threshold
		m_fMeanCriticalCardiacArrestHeartRateThresholdLowBPM = 0.5 * (m_fCriticalHeartRateThresholdLowBPM + m_fCardiacArrestHeartRateThresholdLowBPM);
		
		ResetVitalsToDefault();
		
		// Only run the update loop on the server
		if (Replication.IsServer())
		{
			SetEventMask(owner, EntityEvent.FRAME);
			m_pDamageManager.GetOnDamageStateChanged().Insert(OnDamageStateChanged);
			m_pCharacterController.m_OnLifeStateChanged.Insert(OnLifeStateChanged);		
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
		m_fVitalsUpdateTimerS += timeSlice;
		
		if (m_fVitalsUpdateTimerS < m_fVitalsUpdateTimeoutS)
			return;
		
		UpdateVitals(m_fVitalsUpdateTimerS);
		m_fVitalsUpdateTimerS = 0;
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateVitals(float timeSlice)
	{
		UpdateHeartRate(timeSlice);
		UpdateCardiacOutput(timeSlice);
		UpdateSystemicVascularResistance(timeSlice);
		UpdateBloodPressures(timeSlice);
		UpdateVitalState(timeSlice);
		UpdateResilienceRecoveryScale(timeSlice);
		
		if (GetOwner() == SCR_PlayerController.GetLocalMainEntity())
		{
			Tuple2<float, float> pressures = GetBloodPressures();
			PrintFormat("| %1 BPM | %2 mmHg | %3 mmHg | %4 ml/s |", m_fHeartRateBPM, pressures.param1 * KPA2MMHG, pressures.param2 * KPA2MMHG, m_pDamageManager.ACE_Medical_GetMaxTotalBleedingRate());
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateHeartRate(float timeSlice)
	{
		if (IsInCardiacArrest())
		{
			m_fHeartRateBPM = ComputeCardiacArrestHeartRate();
			return;
		}
		
		float targetHeartRate = ComputeTargetHeartRate();
		float heartRateAcceleration;
		
		if (m_pBloodHZ.GetHealthScaled() >= m_pBloodHZ.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_4_HEMORRHAGE))
		{
			heartRateAcceleration = 0.5 * Math.Round(targetHeartRate - m_fHeartRateBPM);
		} else
		{
			heartRateAcceleration = - Math.Round(m_fHeartRateBPM / 10);
		}
		
		if (heartRateAcceleration >= 0)
		{
			m_fHeartRateBPM = Math.Min(targetHeartRate, m_fHeartRateBPM + heartRateAcceleration * timeSlice);
		}
		else
		{
			m_fHeartRateBPM = Math.Max(targetHeartRate, m_fHeartRateBPM + heartRateAcceleration * timeSlice);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateCardiacOutput(float timeSlice)
	{
		m_fCardiacOutput = m_fHeartRateBPM * m_fDefaultStrokeVolumeML * m_pBloodHZ.GetHealthScaled();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateSystemicVascularResistance(float timeSlice)
	{
		m_fSystemicVascularResistance = m_fDefaultSystemicVascularResistance; // TO DO: medication
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateBloodPressures(float timeSlice)
	{
		m_fMeanArterialPressureKPA = m_fCardiacOutput * m_fSystemicVascularResistance;
		m_fPulsePressureKPA = m_fPulsePressureScale * m_fMeanArterialPressureKPA;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateVitalState(float timeSlice)
	{
		ACE_Medical_EVitalState vitalState = ACE_Medical_EVitalState.NORMAL;
		
		// Check for CPR success in case we are in cardiac arrest
		if (IsInCardiacArrest())
		{
			if (m_bIsCPRPerformed)
			{
				m_fCPRSuccessCheckTimerS += timeSlice;
				
				if (m_fCPRSuccessCheckTimerS < m_fCPRSuccessCheckTimeoutS)
					return;
				
				m_fCPRSuccessCheckTimerS = 0;
				
				if (ComputeCPRSuccess())
					Revive();
			}
			
			// Skip the rest, since we either stay in cardiac arrest or the revival will update the vital state already
			return;
		}
		// Check for vitals that lead to cardiac arrest
		if ((m_fHeartRateBPM < m_fCardiacArrestHeartRateThresholdLowBPM || m_fHeartRateBPM > m_fCardiacArrestHeartRateThresholdHighBPM) ||
			(m_fPulsePressureKPA < m_fCardiacArrestMeanArterialPressureThresholdLowKPA && m_fHeartRateBPM < m_fCriticalHeartRateThresholdLowBPM) ||
			(m_fPulsePressureKPA > m_fCardiacArrestMeanArterialPressureThresholdHighKPA) ||
			m_pBloodHZ.GetHealthScaled() < m_pBloodHZ.GetDamageStateThreshold(m_eCardiacArrestBloodLevelThreshold)
		)
		{
			vitalState = ACE_Medical_EVitalState.CARDIAC_ARREST;
		}
		// Random chance to enter cardiac arrest when heart rate is below the mean of critical and cardiac arrest threshold
		else if (m_fHeartRateBPM < m_fMeanCriticalCardiacArrestHeartRateThresholdLowBPM)
		{
			bool enterCardiacArrest = false;
			m_fCheckCriticalHeartRateTimerS += timeSlice;
			
			if (m_fCheckCriticalHeartRateTimerS >= m_fCheckCriticalHeartRateTimeoutS)
			{
				m_fCheckCriticalHeartRateTimerS = 0;
				
				if (Math.RandomFloat01() < (0.4 + 0.6  * (m_fMeanCriticalCardiacArrestHeartRateThresholdLowBPM - m_fHeartRateBPM) / 10))
					enterCardiacArrest = true;	
			}

			if (enterCardiacArrest)
				vitalState = ACE_Medical_EVitalState.CARDIAC_ARREST;
			else
				vitalState = ACE_Medical_EVitalState.CRITICAL;
		}
		// Check for vitals that lead to critical state (causes fainting)
		// TO DO: Also handle the case of knockout bleeding rate
		else if ((m_fHeartRateBPM < m_fCriticalHeartRateThresholdLowBPM) || (m_fHeartRateBPM > m_fCriticalHeartRateThresholdHighBPM) ||
			(m_fMeanArterialPressureKPA < m_fCriticalMeanArterialPressureThresholdLowKPA) || (m_fMeanArterialPressureKPA > m_fCriticalMeanArterialPressureThresholdHighKPA) ||
			m_pBloodHZ.GetHealthScaled() < m_pBloodHZ.GetDamageStateThreshold(m_eCriticalBloodLevelThreshold)
		)
		{
			vitalState = ACE_Medical_EVitalState.CRITICAL;
		}
		/*
		// TO DO: Decide whether we even have any use for ACE_Medical_EVitalState.LOWERED
		// Check for vitals that lead to lowered vital state
		else if (m_pDamageManager.ACE_Medical_GetPainIntensity() > 0 || m_pDamageManager.IsBleeding())
		{
			vitalState = ACE_Medical_EVitalState.LOWERED;
		}
		*/
				
		SetVitalState(vitalState);
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateResilienceRecoveryScale(float timeSlice)
	{
		// Resilience cannot recover while in cardiac arrest or when blood level is too low
		if (IsInCardiacArrest() || (m_pBloodHZ.GetHealthScaled() < m_pBloodHZ.GetDamageStateThreshold(m_eMinBloodLevelForResilienceRecovery)))
			m_fResilienceRecoveryScale = 0;
		// Reduced resilience rate after revival. Recovery is harder the lower the brain's health is.
		else if (m_bWasInCardiacArrest)
			m_fResilienceRecoveryScale = m_fMaxRevivalResilienceRecoveryScale * m_pBrainHZ.GetHealthScaled();
		else
			m_fResilienceRecoveryScale = 1;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Terminate vitals loop when killed
	protected void OnDamageStateChanged(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		
		m_fHeartRateBPM = 0;
		m_fCardiacOutput = 0;
		m_fMeanArterialPressureKPA = 0;
		m_fPulsePressureKPA = 0;
		SetVitalState(ACE_Medical_EVitalState.CARDIAC_ARREST);
		
		ClearEventMask(GetOwner(), EntityEvent.FRAME);
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeCardiacArrestHeartRate()
	{
		if (m_bIsCPRPerformed)
		{
			return 100 + Math.Clamp(Math.AbsFloat(Math.RandomGaussFloat(20/3, 0)), 0, 20);
		}
		
		return 0;
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeTargetHeartRate()
	{
		float target = m_fDefaultHeartRateBPM;
		
		if (m_pBloodHZ.GetHealthScaled() < m_pBloodHZ.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_3_HEMORRHAGE))
			target = m_fHeartRateBPM * 14.3 * Math.Lerp(0.5, 1, m_pBloodHZ.GetHealthScaled()) / Math.Max(6, m_fMeanArterialPressureKPA);
		
		target = Math.Max(target, m_fDefaultHeartRateBPM + 50 * m_pDamageManager.ACE_Medical_GetPainIntensity());
				
		// TO DO: Do SpO2 and medication

		
		return Math.Max(0, target);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnVitalStateChanged(ACE_Medical_EVitalState newState, ACE_Medical_EVitalState prevState)
	{
		m_pDamageManager.UpdateConsciousness();
		
		// Kill AI that enters cardiac arrest
		if (IsInCardiacArrest() && !SCR_EntityHelper.IsAPlayer(GetOwner()))
			m_pDamageManager.Kill(m_pDamageManager.GetInstigator());
		
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
			m_pDamageManager.GetResilienceHitZone().SetHealth(0);
			m_pDamageManager.AddDamageEffect(m_CardiacArrestDamageEffect);
			m_bWasInCardiacArrest = true;
		}
		else
		{
			m_pDamageManager.TerminateDamageEffectsOfType(ACE_Medical_CardiacArrestDamageEffect);
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
			m_pDamageManager.TerminateDamageEffectsOfType(ACE_Medical_CardiacArrestDamageEffect);
		else
			m_pDamageManager.AddDamageEffect(m_CardiacArrestDamageEffect);
		
	}
	
	//------------------------------------------------------------------------------------------------
	void Revive()
	{		
		m_fHeartRateBPM = m_fCriticalHeartRateThresholdLowBPM;
		SetVitalState(ACE_Medical_EVitalState.CRITICAL);
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetVitalsToDefault()
	{
		m_fHeartRateBPM = m_fDefaultHeartRateBPM;
		UpdateCardiacOutput(0);
		UpdateSystemicVascularResistance(0);
		UpdateBloodPressures(0);
		SetVitalState(ACE_Medical_EVitalState.NORMAL);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool ComputeCPRSuccess()
	{
		// TO DO: Scale success based on HR medication
		float minScale = m_pBloodHZ.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_4_HEMORRHAGE);
		float maxScale = m_pBloodHZ.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_2_HEMORRHAGE);
		float scale = Math.Clamp(m_pBloodHZ.GetHealthScaled(), minScale, maxScale);
		return (Math.RandomFloat01() < Math.Map(scale, minScale, maxScale, m_fCPRSuccessChanceMin, m_fCPRSuccessChanceMax));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Resilience recovery scale based on vitals
	float GetResilienceRecoveryScale()
	{
		return m_fResilienceRecoveryScale;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetMaxRevivalResilienceRecoveryScale()
	{
		return m_fMaxRevivalResilienceRecoveryScale;
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
	//! Returns cardiac output in ml/min
	float GetCardiacOutput()
	{
		return m_fCardiacOutput;
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
	ScriptInvokerInt2 GetOnVitalStateChanged()
	{
		return m_OnVitalStateChanged;
	}
}

//------------------------------------------------------------------------------------------------
enum ACE_Medical_EVitalState
{
	NORMAL,
	LOWERED,
	CRITICAL,
	CARDIAC_ARREST
}
