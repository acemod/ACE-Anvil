//------------------------------------------------------------------------------------------------
//! Updates to vitals are mostly server side right now.
//! Clients can request values for vitals via ACE_Medical_NetworkComponent
//! TO DO: Inherit from ACE_Medical_BaseSystem once systems support inheritance
class ACE_Medical_RespiratorySystem : ACE_Medical_BaseSystem3
{
	protected ACE_Medical_RespiratorySystemSettings m_Settings;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (settings && settings.m_RespiratorySystem)
			m_Settings = settings.m_RespiratorySystem;
		
		super.OnInit();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnStart(IEntity entity)
	{
		ACE_Medical_RespiratoryComponent component = ACE_Medical_RespiratoryComponent.Cast(entity.FindComponent(ACE_Medical_RespiratoryComponent));
		if (!component)
			return;
		
		ResetVitalsToDefault(component);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnStop(IEntity entity)
	{
		ACE_Medical_RespiratoryComponent component = ACE_Medical_RespiratoryComponent.Cast(entity.FindComponent(ACE_Medical_RespiratoryComponent));
		if (!component)
			return;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void Update(IEntity entity, float timeSlice)
	{
		super.Update(entity, timeSlice);
		
		ACE_Medical_RespiratoryComponent component = ACE_Medical_RespiratoryComponent.Cast(entity.FindComponent(ACE_Medical_RespiratoryComponent));
		if (!component)
			return;
		
		ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(entity.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!cardiovascularComponent)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(entity.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!damageManager)
			return;	
		
		UpdateVentilation(component, cardiovascularComponent, damageManager, timeSlice);
		
		if (m_Settings.m_bPaCO2Enabled)
			UpdatePaCO2(component, timeSlice);
		
		if (m_Settings.m_bKidneyEnabled)
			UpdatePH(component, timeSlice);
		
		UpdatePaO2(component, damageManager, timeSlice);
		UpdateSpO2(component, cardiovascularComponent, timeSlice);	
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateVentilation(ACE_Medical_RespiratoryComponent component, ACE_Medical_CardiovascularComponent cardiovascularComponent, SCR_CharacterDamageManagerComponent damageManager, float timeSlice)
	{
		float anaerobicPressure = Math.Max(0.8 / damageManager.GetBloodHitZone().GetHealthScaled(), 1.2);
		component.SetAnaerobicPressure(anaerobicPressure);
		float tidalVolume = m_Settings.m_fDefaultTidalVolumeML - component.GetPneumothoraxVolume();
		
		float targetVentilation = m_Settings.m_fMinTargetVentilationMLPM;
		if (!cardiovascularComponent.IsInCardiacArrest())
			targetVentilation = 60 * cardiovascularComponent.GetHeartRate() / anaerobicPressure + Math.Max(1500 * (component.GetPaCO2() - m_Settings.m_fDefaultPaCO2KPA), targetVentilation);
		
		component.SetTargetVentilation(targetVentilation);
		
		// TO DO: Include BVM
		float respiratoryRate = 0;
		if (!cardiovascularComponent.IsInCardiacArrest())
		{
			// TO DO: Include adjustements from medication
			respiratoryRate = targetVentilation / tidalVolume /* - 5 * MED */;
			
			if (component.GetPaCO2() > m_Settings.m_fMinPaCO2RRBoostKPA)
				respiratoryRate += 1.5 * (component.GetPaCO2() - m_Settings.m_fMinPaCO2RRBoostKPA);
			
			respiratoryRate = Math.Min(respiratoryRate, m_Settings.m_fMaxRespiratoryRateBPM);
		}
		
		component.SetRespiratoryRate(respiratoryRate);
		
		float ventilation = 1;
		if (!cardiovascularComponent.IsInCardiacArrest())
			ventilation = tidalVolume * respiratoryRate;
		
		component.SetVentilation(ventilation);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdatePaCO2(ACE_Medical_RespiratoryComponent component, float timeSlice)
	{
		float ventRatio = component.GetTargetVentilation() / component.GetVentilation();
		
		float paCO2Rate;
		if (float.AlmostEqual(ventRatio, 1, epsilon: 1e-3))
			paCO2Rate = Math.Clamp(m_Settings.m_fDefaultPaCO2KPA - component.GetPaCO2() + 20 * Math.Max(component.GetAnaerobicPressure() - 1.2, 0), -m_Settings.m_fMaxPaCO2RateKPAPS, m_Settings.m_fMaxPaCO2RateKPAPS);
		else if (ventRatio > 1)
			paCO2Rate = m_Settings.m_fMaxPaCO2RateKPAPS;
		else
			paCO2Rate = -m_Settings.m_fMaxPaCO2RateKPAPS;
		
		component.SetPaCO2(component.GetPaCO2() + paCO2Rate * timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	//! TO DO: Implement pH calculation
	protected void UpdatePH(ACE_Medical_RespiratoryComponent component, float timeSlice)
	{
		//m_Settings.m_fpKaCarbonicAcid + Math.Log10(/);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdatePaO2(ACE_Medical_RespiratoryComponent component, SCR_CharacterDamageManagerComponent damageManager, float timeSlice)
	{
		float fiO2 = m_Settings.m_fDefaultFiO2;
		// TO DO: Handle oxygen tank
		if (component.HasTensionPneumothorax() || component.HasHemothorax() || component.AreAirwaysBlocked())
			fiO2 = 0;
		
		// TO DO: Get atmospheric pressure from an external source
		float palvO2 = Math.Max(fiO2 * (101.325 - 6.266) - component.GetPaCO2() / component.GetAnaerobicPressure(), 0.133);
		float targetPaO2 = Math.Min(m_Settings.m_fDefaultPaO2KPA - (component.GetTargetVentilation() - component.GetVentilation()) / damageManager.GetBloodHitZone().GetHealthScaled() / 900, palvO2);
		
		float prevPaO2 = component.GetPaO2();
		if (prevPaO2 > targetPaO2)
			component.SetPaO2(Math.Max(targetPaO2, prevPaO2 - m_Settings.m_fMaxPaO2RateKPAPS * timeSlice));
		else
			component.SetPaO2(Math.Min(targetPaO2, prevPaO2 + m_Settings.m_fMaxPaO2RateKPAPS * timeSlice));
		
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateSpO2(ACE_Medical_RespiratoryComponent component, ACE_Medical_CardiovascularComponent cardiovascularComponent, float timeSlice)
	{
		cardiovascularComponent.SetSpO2(100 * ACE_Math.Hill(Math.Pow(component.GetPaO2(), 2.7) / component.GetHemoglobinKd()))
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnFullHeal(IEntity entity)
	{
		super.OnFullHeal(entity);
		
		ACE_Medical_RespiratoryComponent component = ACE_Medical_RespiratoryComponent.Cast(entity.FindComponent(ACE_Medical_RespiratoryComponent));
		if (!component)
			return;
		
		ResetVitalsToDefault(component);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ResetVitalsToDefault(ACE_Medical_RespiratoryComponent component)
	{
		component.SetPaCO2(m_Settings.m_fDefaultPaCO2KPA);
		component.SetPaO2(m_Settings.m_fDefaultPaO2KPA);
		component.SetPH(m_Settings.m_fDefaultpH);
		component.SetHemoglobinKd(m_Settings.m_fDefaultHemoglobinKd);
		component.SetAreAirwayBlocked(false);
		component.SetPneumothoraxVolume(0);
		component.SetHasTensionPneumothorax(false);
		component.SetHasHemothorax(false);
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
		
		ACE_Medical_RespiratoryComponent component = ACE_Medical_RespiratoryComponent.Cast(target.FindComponent(ACE_Medical_RespiratoryComponent));
		if (!component)
			return;

		DbgUI.Begin(string.Format("ACE_Medical_RespiratoryComponent (%1)", targetType), 0, 700);
		DbgUI.Text(string.Format("Target ventilation:           %1 ml/min", component.GetTargetVentilation()));
		DbgUI.Text(string.Format("Actual ventilation:           %1 ml/min", component.GetVentilation()));
		DbgUI.Text(string.Format("Respiratory rate:              %1 1/min", component.GetRespiratoryRate()));
		DbgUI.Text(string.Format("PaCO2:                        %1 kPa", component.GetPaCO2()));
		DbgUI.Text(string.Format("PaO2:                         %1 kPa", component.GetPaO2()));
		DbgUI.Text(string.Format("pH:                           %1", component.GetPH()));
		DbgUI.Text(string.Format("Airways blocked:              %1", component.AreAirwaysBlocked().ToString()));
		DbgUI.Text(string.Format("Pneumothorax volume:          %1 ml", component.GetPneumothoraxVolume()));
		DbgUI.Text(string.Format("Tension pneumothorax:         %1", component.HasTensionPneumothorax().ToString()));
		DbgUI.Text(string.Format("Hemothorax:                   %1", component.HasHemothorax().ToString()));
		DbgUI.End();
	}
#endif
}
