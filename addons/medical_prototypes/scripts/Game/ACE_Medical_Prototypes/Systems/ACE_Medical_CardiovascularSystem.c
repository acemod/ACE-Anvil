//------------------------------------------------------------------------------------------------
class ACE_Medical_CardiovascularSystem : ACE_Medical_BaseSystem
{
	protected ACE_Medical_CardiovascularSystemSettings m_Settings;
	
	protected float m_fCPRSuccessCheckTimerS = 0;
	
	static const float KPA2MMHG = 7.50062;
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(false);
	}
	
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
		if (!entity)
			return;
		
		ACE_Medical_CardiovascularComponent component = ACE_Medical_CardiovascularComponent.Cast(entity.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!component)
			return;
		
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.GetEditableEntity(entity);
		// Don't handle entities that are about to get deleted
		if (!editableEntity || !editableEntity.IsRegistered())
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
		component.SetSystemicVascularResistance(m_Settings.m_fDefaultSystemicVascularResistance + component.GetSystemicVascularResistenceMedicationAdjustment());
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
		ACE_Medical_EVitalState vitalState = ACE_Medical_EVitalState.STABLE;
		SCR_CharacterBloodHitZone bloodHZ = damageManager.GetBloodHitZone();
		float heartRate = component.GetHeartRate();
		float meanArterialPressure = component.GetMeanArterialPressure();
		
		// Check for CPR success in case we are in cardiac arrest
		if (component.IsInCardiacArrest())
		{
			if (component.IsCPRPerformed())
			{
				// Cannot revive when MAP is too high
				if (meanArterialPressure > m_Settings.m_CardiacArrestThresholds.m_fMeanArterialPressureHighKPA)
				{
					// Prevent BP from getting absurdly high through medication
					component.SetMeanArterialPressure(m_Settings.m_CardiacArrestThresholds.m_fMeanArterialPressureHighKPA);
					component.SetPulsePressure(m_Settings.m_fPulsePressureScale * m_Settings.m_CardiacArrestThresholds.m_fMeanArterialPressureHighKPA);
					return;
				}
				
				m_fCPRSuccessCheckTimerS += timeSlice * component.GetReviveSuccessCheckTimerScale();
				
				if (m_fCPRSuccessCheckTimerS < m_Settings.m_fCPRSuccessCheckTimeoutS)
					return;
				
				m_fCPRSuccessCheckTimerS = 0;
				
				if (ComputeCPRSuccess(component, damageManager))
					component.Revive();
			}
			
			// Skip the rest, since we either stay in cardiac arrest or the revival will update the vital state already
			return;
		}
		// Check for vitals that lead to cardiac arrest
		if ((heartRate < m_Settings.m_CardiacArrestThresholds.m_fHeartRateLowBPM) ||
			(heartRate > m_Settings.m_CardiacArrestThresholds.m_fHeartRateHighBPM) ||
			(meanArterialPressure < m_Settings.m_CardiacArrestThresholds.m_fMeanArterialPressureLowKPA && heartRate < m_Settings.m_CriticalThresholds.m_fHeartRateLowBPM) ||
			(meanArterialPressure > m_Settings.m_CardiacArrestThresholds.m_fMeanArterialPressureHighKPA) ||
			bloodHZ.GetHealthScaled() <= bloodHZ.GetDamageStateThreshold(m_Settings.m_CardiacArrestThresholds.m_eBloodState)
		)
		{
			vitalState = ACE_Medical_EVitalState.CARDIAC_ARREST;
		}
		// Check for vitals that lead to critical vital state
		// TO DO: Also handle the case of knockout bleeding rate
		else if ((heartRate < m_Settings.m_CriticalThresholds.m_fHeartRateLowBPM) ||
				(heartRate > m_Settings.m_CriticalThresholds.m_fHeartRateHighBPM) ||
				(meanArterialPressure < m_Settings.m_CriticalThresholds.m_fMeanArterialPressureLowKPA) ||
				(meanArterialPressure > m_Settings.m_CriticalThresholds.m_fMeanArterialPressureHighKPA) ||
				bloodHZ.GetHealthScaled() <= bloodHZ.GetDamageStateThreshold(m_Settings.m_CriticalThresholds.m_eBloodState)
		)
		{
			vitalState = ACE_Medical_EVitalState.CRITICAL;
		}
		// Check for vitals that lead to unstable state (suppresses resilience recovery)
		else if ((heartRate < m_Settings.m_UnstableThresholds.m_fHeartRateLowBPM) ||
				(heartRate > m_Settings.m_UnstableThresholds.m_fHeartRateHighBPM) ||
				(meanArterialPressure < m_Settings.m_UnstableThresholds.m_fMeanArterialPressureLowKPA) ||
				(meanArterialPressure > m_Settings.m_UnstableThresholds.m_fMeanArterialPressureHighKPA) ||
				bloodHZ.GetHealthScaled() <= bloodHZ.GetDamageStateThreshold(m_Settings.m_UnstableThresholds.m_eBloodState)
		)
		{
			vitalState = ACE_Medical_EVitalState.UNSTABLE;
		}
				
		component.SetVitalState(vitalState);
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateResilienceRecoveryScale(ACE_Medical_CardiovascularComponent component, SCR_CharacterDamageManagerComponent damageManager, float timeSlice)
	{
		SCR_CharacterBloodHitZone bloodHZ = damageManager.GetBloodHitZone();
		ACE_Medical_BrainHitZone brainHZ = damageManager.ACE_Medical_GetBrainHitZone();
		
		// Resilience cannot recover while in vital state is unstable
		if (component.GetVitalState() >= ACE_Medical_EVitalState.UNSTABLE)
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
		target += component.GetHeartRateMedicationAdjustment();
		// TO DO: Do SpO2
		
		return Math.Max(0, target);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool ComputeCPRSuccess(ACE_Medical_CardiovascularComponent component, SCR_CharacterDamageManagerComponent damageManager)
	{
		SCR_CharacterBloodHitZone bloodHZ = damageManager.GetBloodHitZone();
		
		float minScale = bloodHZ.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_4_HEMORRHAGE);
		float maxScale = bloodHZ.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_2_HEMORRHAGE);
		float scale = Math.Clamp(bloodHZ.GetHealthScaled(), minScale, maxScale);
		return (Math.RandomFloat01() < Math.Map(scale, minScale, maxScale, m_Settings.m_fCPRSuccessChanceMin, m_Settings.m_fCPRSuccessChanceMax));
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ResetVitalsToDefault(ACE_Medical_CardiovascularComponent component, SCR_CharacterDamageManagerComponent damageManager)
	{
		component.SetHeartRate(m_Settings.m_fDefaultHeartRateBPM);
		UpdateCardiacOutput(component, damageManager, 0);
		UpdateSystemicVascularResistance(component, damageManager, 0);
		UpdateBloodPressures(component, damageManager, 0);
		component.SetVitalState(ACE_Medical_EVitalState.STABLE);
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

#ifdef WORKBENCH
	//------------------------------------------------------------------------------------------------
	//! Show vitals for target entity
	override protected void OnDiag(float timeSlice)
	{
		super.OnDiag(timeSlice);
		
		IEntity target;
		string targetType;
		if (!GetDiagTarget(target, targetType))
			return;
		
		ACE_Medical_CardiovascularComponent component = ACE_Medical_CardiovascularComponent.Cast(target.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!component)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(target.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!damageManager)
			return;

		DbgUI.Begin(string.Format("ACE_Medical_CardiovascularSystem (%1)", targetType), 0, 700);
		DbgUI.Text(string.Format("Vital state:                  %1", SCR_Enum.GetEnumName(ACE_Medical_EVitalState, component.GetVitalState())));
		DbgUI.Text(string.Format("Heart rate:                   %1 bpm", Math.Round(component.GetHeartRate())));
		Tuple2<float, float> pressures = component.GetBloodPressures();
		DbgUI.Text(string.Format("Blood pressure:               %1/%2 mmHg", Math.Round(pressures.param2 * KPA2MMHG), Math.Round(pressures.param1 * KPA2MMHG)));
		DbgUI.Text(string.Format("Blood state:                  %1", SCR_Enum.GetEnumName(ACE_Medical_EBloodState, damageManager.GetBloodHitZone().GetDamageState())));
		DbgUI.Text(string.Format("Resilience recovery scale:    %1", component.GetResilienceRecoveryScale()));
		DbgUI.End();
	}
#endif
}
