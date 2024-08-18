//------------------------------------------------------------------------------------------------
class ACE_Medical_CardiovascularSystem : ACE_Medical_BaseSystem
{
	protected ACE_Medical_CardiovascularSystemSettings m_Settings;
	
	protected float m_fCPRSuccessCheckTimerS = 0;
	protected float m_fCheckCriticalHeartRateTimeoutS = 5;
	protected float m_fCheckCriticalHeartRateTimerS = 0;
	protected bool m_bInitDone = false;
	
	static const float KPA2MMHG = 7.50062;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (settings && settings.m_CardiovascularSystem)
			m_Settings = settings.m_CardiovascularSystem;
		
		super.OnInit();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnStart(IEntity entity)
	{
		ACE_Medical_CardiovascularComponent component = ACE_Medical_CardiovascularComponent.Cast(entity.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!component)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(entity.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!damageManager)
			return;
		
		ResetVitalsToDefault(component, damageManager);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnStop(IEntity entity)
	{
		ACE_Medical_CardiovascularComponent component = ACE_Medical_CardiovascularComponent.Cast(entity.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!component)
			return;
		
		component.SetHeartRate(0);
		component.SetCardiacOutput(0);
		component.SetMeanArterialPressure(0);
		component.SetPulsePressure(0);
		component.SetVitalState(ACE_Medical_EVitalState.CARDIAC_ARREST);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void Update(IEntity entity, float timeSlice)
	{
		super.Update(entity, timeSlice);
		
		ACE_Medical_CardiovascularComponent component = ACE_Medical_CardiovascularComponent.Cast(entity.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!component)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(entity.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!damageManager)
			return;
		
		UpdateHeartRate(component, damageManager, timeSlice);
		UpdateCardiacOutput(component, damageManager, timeSlice);
		UpdateSystemicVascularResistance(component, damageManager, timeSlice);
		UpdateBloodPressures(component, damageManager, timeSlice);
		UpdateVitalState(component, damageManager, timeSlice);
		UpdateResilienceRecoveryScale(component, damageManager, timeSlice);
		
		if (component.GetOwner() == SCR_PlayerController.GetLocalMainEntity())
		{
			float heartRate = component.GetHeartRate();
			Tuple2<float, float> pressures = component.GetBloodPressures();
			PrintFormat("| %1 BPM | %2 mmHg | %3 mmHg |", heartRate, pressures.param1 * KPA2MMHG, pressures.param2 * KPA2MMHG);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateHeartRate(ACE_Medical_CardiovascularComponent component, SCR_CharacterDamageManagerComponent damageManager, float timeSlice)
	{
		SCR_CharacterBloodHitZone bloodHZ = damageManager.GetBloodHitZone();
		
		if (component.IsInCardiacArrest())
		{
			component.SetHeartRate(ComputeCardiacArrestHeartRate(component, damageManager));
			return;
		}
		
		float targetHeartRate = ComputeTargetHeartRate(component, damageManager);
		float prevHeartRate = component.GetHeartRate();
		float heartRateAcceleration;
		
		if (bloodHZ.GetHealthScaled() >= bloodHZ.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_4_HEMORRHAGE))
		{
			heartRateAcceleration = 0.5 * Math.Round(targetHeartRate - prevHeartRate);
		} else
		{
			heartRateAcceleration = - Math.Round(prevHeartRate / 10);
		}
		
		if (heartRateAcceleration >= 0)
		{
			component.SetHeartRate(Math.Min(targetHeartRate, prevHeartRate + heartRateAcceleration * timeSlice));
		}
		else
		{
			component.SetHeartRate(Math.Max(targetHeartRate, prevHeartRate + heartRateAcceleration * timeSlice));
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateCardiacOutput(ACE_Medical_CardiovascularComponent component, SCR_CharacterDamageManagerComponent damageManager, float timeSlice)
	{
		component.SetCardiacOutput(component.GetHeartRate() * m_Settings.m_fDefaultStrokeVolumeML * damageManager.GetBloodHitZone().GetHealthScaled());
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateSystemicVascularResistance(ACE_Medical_CardiovascularComponent component, SCR_CharacterDamageManagerComponent damageManager, float timeSlice)
	{
		component.SetSystemicVascularResistance(m_Settings.m_fDefaultSystemicVascularResistance); // TO DO: medication
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateBloodPressures(ACE_Medical_CardiovascularComponent component, SCR_CharacterDamageManagerComponent damageManager, float timeSlice)
	{
		float meanArterialPressure = component.GetCardiacOutput() * component.GetSystemicVascularResistance();
		component.SetMeanArterialPressure(meanArterialPressure);
		component.SetPulsePressure(m_Settings.m_fPulsePressureScale * meanArterialPressure);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateVitalState(ACE_Medical_CardiovascularComponent component, SCR_CharacterDamageManagerComponent damageManager, float timeSlice)
	{
		ACE_Medical_EVitalState vitalState = ACE_Medical_EVitalState.NORMAL;
		SCR_CharacterBloodHitZone bloodHZ = damageManager.GetBloodHitZone();
		float heartRate = component.GetHeartRate();
		float meanArterialPressure = component.GetMeanArterialPressure();
		
		
		// Check for CPR success in case we are in cardiac arrest
		if (component.IsInCardiacArrest())
		{
			// Check time spent in cardiac arrest so far + change rhythm 
			UpdateArrestCardiacRhythmState(component);
			
			// Process revive chance
			if (component.IsCPRPerformed())
			{
				m_fCPRSuccessCheckTimerS += timeSlice;
				
				if (m_fCPRSuccessCheckTimerS < m_Settings.m_fCPRSuccessCheckTimeoutS)
					return;
				
				m_fCPRSuccessCheckTimerS = 0;
				
				float reviveChance = ComputeReviveChance(component, damageManager);				
				if (Math.RandomFloat01() < reviveChance)
					Revive(component);
			}
			
			// Skip the rest, since we either stay in cardiac arrest or the revival will update the vital state already
			return;
		}
		// Check for vitals that lead to cardiac arrest
		if ((heartRate < m_Settings.m_fCardiacArrestHeartRateThresholdLowBPM || heartRate > m_Settings.m_fCardiacArrestHeartRateThresholdHighBPM) ||
			(meanArterialPressure < m_Settings.m_fCardiacArrestMeanArterialPressureThresholdLowKPA && heartRate < m_Settings.m_fCriticalHeartRateThresholdLowBPM) ||
			(meanArterialPressure > m_Settings.m_fCardiacArrestMeanArterialPressureThresholdHighKPA) ||
			bloodHZ.GetHealthScaled() < bloodHZ.GetDamageStateThreshold(m_Settings.m_eCardiacArrestBloodLevelThreshold)
		)
		{
			vitalState = ACE_Medical_EVitalState.CARDIAC_ARREST;
		}
		// Random chance to enter cardiac arrest when heart rate is below the mean of critical and cardiac arrest threshold
		else if (heartRate < m_Settings.m_fMeanCriticalCardiacArrestHeartRateThresholdLowBPM)
		{
			bool enterCardiacArrest = false;
			m_fCheckCriticalHeartRateTimerS += timeSlice;
			
			if (m_fCheckCriticalHeartRateTimerS >= m_fCheckCriticalHeartRateTimeoutS)
			{
				m_fCheckCriticalHeartRateTimerS = 0;
				
				if (Math.RandomFloat01() < (0.4 + 0.6  * (m_Settings.m_fMeanCriticalCardiacArrestHeartRateThresholdLowBPM - heartRate) / 10))
					enterCardiacArrest = true;
			}

			if (enterCardiacArrest)
				vitalState = ACE_Medical_EVitalState.CARDIAC_ARREST;
			else
				vitalState = ACE_Medical_EVitalState.CRITICAL;
		}
		// Check for vitals that lead to critical state (causes fainting)
		// TO DO: Also handle the case of knockout bleeding rate
		else if ((heartRate < m_Settings.m_fCriticalHeartRateThresholdLowBPM) || (heartRate > m_Settings.m_fCriticalHeartRateThresholdHighBPM) ||
			(meanArterialPressure < m_Settings.m_fCriticalMeanArterialPressureThresholdLowKPA) || (meanArterialPressure > m_Settings.m_fCriticalMeanArterialPressureThresholdHighKPA) ||
			bloodHZ.GetHealthScaled() < bloodHZ.GetDamageStateThreshold(m_Settings.m_eCriticalBloodLevelThreshold)
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
				
		component.SetVitalState(vitalState);
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateResilienceRecoveryScale(ACE_Medical_CardiovascularComponent component, SCR_CharacterDamageManagerComponent damageManager, float timeSlice)
	{
		SCR_CharacterBloodHitZone bloodHZ = damageManager.GetBloodHitZone();
		ACE_Medical_BrainHitZone brainHZ = damageManager.ACE_Medical_GetBrainHitZone();
		
		// Resilience cannot recover while in cardiac arrest or when blood level is too low
		if (component.IsInCardiacArrest() || (bloodHZ.GetHealthScaled() < bloodHZ.GetDamageStateThreshold(m_Settings.m_eMinBloodLevelForResilienceRecovery)))
			component.SetResilienceRecoveryScale(0);
		// Reduced resilience rate after revival. Recovery is harder the lower the brain's health is.
		else if (component.WasInCardiacArrest())
			component.SetResilienceRecoveryScale(m_Settings.m_fMaxRevivalResilienceRecoveryScale * brainHZ.GetHealthScaled());
		else
			component.SetResilienceRecoveryScale(1);
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeCardiacArrestHeartRate(ACE_Medical_CardiovascularComponent component, SCR_CharacterDamageManagerComponent damageManager)
	{
		if (component.IsCPRPerformed())
		{
			return 100 + Math.Clamp(Math.AbsFloat(Math.RandomGaussFloat(20/3, 0)), 0, 20);
		}
		
		return 0;
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeTargetHeartRate(ACE_Medical_CardiovascularComponent component, SCR_CharacterDamageManagerComponent damageManager)
	{
		float target = m_Settings.m_fDefaultHeartRateBPM;
		SCR_CharacterBloodHitZone bloodHZ = damageManager.GetBloodHitZone();
		
		if (bloodHZ.GetHealthScaled() < bloodHZ.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_3_HEMORRHAGE))
			target = component.GetHeartRate() * 14.3 * Math.Lerp(0.5, 1, bloodHZ.GetHealthScaled()) / Math.Max(6, component.GetMeanArterialPressure());
		
		target = Math.Max(target, m_Settings.m_fDefaultHeartRateBPM + 50 * damageManager.ACE_Medical_GetPainIntensity());
				
		// TO DO: Do SpO2 and medication

		
		return Math.Max(0, target);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateArrestCardiacRhythmState(ACE_Medical_CardiovascularComponent component)
	{
		if (component.GetArrestTimeCurrent() >= 3*60*1000)
			component.SetCardiacRhythmState(ACE_Medical_ECardiacRhythmState.ASYSTOLE)
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeRhythmPenalty(ACE_Medical_CardiovascularComponent component)
	{
		ACE_Medical_ECardiacRhythmState rhythmState = component.GetCardiacRhythmState();
		int diffTime = component.GetArrestTimeCurrent();
		int shockAmount = component.GetShocksDelivered();
		
		switch (rhythmState)
		{
			case ACE_Medical_ECardiacRhythmState.ASYSTOLE:
				return m_Settings.m_fCardiacRhythmsSuccessChanceAsystole;
			case ACE_Medical_ECardiacRhythmState.VF:
				return Math.Map(diffTime, 3*60*1000,
								m_Settings.m_fCardiacRhythmsSuccessChanceAsystole,
								m_Settings.m_fCardiacRhythmsSuccessChanceDefibrillationDelayed,
								m_Settings.m_fCardiacRhythmsSuccessChanceDefibrillationImmediate);
			default:
				return 1;
		}
		
		return 1;
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeShockCountPenalty(ACE_Medical_CardiovascularComponent component)
	{
		int shockAmount = component.GetShocksDelivered();
		
		if (shockAmount > 0)
		{
			return shockAmount * m_Settings.m_fCardiacRhythmsSuccessChanceDefibrillationReduction;
		}
		
		return 1;
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeCPRSuccess(ACE_Medical_CardiovascularComponent component, SCR_CharacterDamageManagerComponent damageManager)
	{
		SCR_CharacterBloodHitZone bloodHZ = damageManager.GetBloodHitZone();
		
		// TO DO: Scale success based on HR medication
		float minScale = bloodHZ.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_4_HEMORRHAGE);
		float maxScale = bloodHZ.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_2_HEMORRHAGE);
		float scale = Math.Clamp(bloodHZ.GetHealthScaled(), minScale, maxScale);
		
		return Math.Map(scale, minScale, maxScale, m_Settings.m_fCPRSuccessChanceMin, m_Settings.m_fCPRSuccessChanceMax);
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeReviveChance(ACE_Medical_CardiovascularComponent component, SCR_CharacterDamageManagerComponent damageManager)
	{
		float cprSuccess = ComputeCPRSuccess(component, damageManager);
		float rhythmPenalty = ComputeRhythmPenalty(component);
		float shockPenalty = ComputeShockCountPenalty(component);
		
		if (m_Settings.m_bCardiacArrestRhythmEnabled)
			return cprSuccess * rhythmPenalty * shockPenalty;
		
		return cprSuccess;
	}
	
	//------------------------------------------------------------------------------------------------
	void Revive(IEntity entity)
	{
		ACE_Medical_CardiovascularComponent component = ACE_Medical_CardiovascularComponent.Cast(entity.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!component)
			return;
		
		Revive(component);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void Revive(ACE_Medical_CardiovascularComponent component)
	{
		component.SetHeartRate(m_Settings.m_fCriticalHeartRateThresholdLowBPM);
		component.SetVitalState(ACE_Medical_EVitalState.CRITICAL);
		component.SetCardiacRhythmState(ACE_Medical_ECardiacRhythmState.SINUS);
		component.ResetShocksDelivered();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ResetVitalsToDefault(ACE_Medical_CardiovascularComponent component, SCR_CharacterDamageManagerComponent damageManager)
	{
		component.SetHeartRate(m_Settings.m_fDefaultHeartRateBPM);
		UpdateCardiacOutput(component, damageManager, 0);
		UpdateSystemicVascularResistance(component, damageManager, 0);
		UpdateBloodPressures(component, damageManager, 0);
		component.SetVitalState(ACE_Medical_EVitalState.NORMAL);
		component.SetCardiacRhythmState(ACE_Medical_ECardiacRhythmState.SINUS);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnFullHeal(IEntity entity)
	{
		super.OnFullHeal(entity);
		
		ACE_Medical_CardiovascularComponent component = ACE_Medical_CardiovascularComponent.Cast(entity.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!component)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(entity.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!damageManager)
			return;
		
		ResetVitalsToDefault(component, damageManager);
	}
}
