//------------------------------------------------------------------------------------------------
modded class ACE_Medical_IVitalState : ACE_FSM_IState<ACE_Medical_CharacterContext>
{
	protected static ACE_Medical_Breathing_Settings s_pBreathingSettings;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_IVitalState(ACE_FSM_EStateID id)
	{
		if (!s_pBreathingSettings)
			s_pBreathingSettings = ACE_SettingsHelperT<ACE_Medical_Breathing_Settings>.GetModSettings();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		context.m_pVitals.SetRespiratoryRate(ComputeRespiratoryRate(context, timeSlice));
		UpdateOxygenMetabolism(context, timeSlice);
		UpdateVentilation(context, timeSlice);
		UpdatePerfusion(context, timeSlice);
		context.m_pVitals.SetPalvO2(ComputePalvO2(context, timeSlice));
		context.m_pVitals.SetSpO2(ComputeSpO2(context, timeSlice));
		context.m_pVitals.SetCvenCO2(ComputeCvenCO2(context, timeSlice));
		context.m_pVitals.SetPalvCO2(ComputePalvCO2(context, timeSlice));
	}
	
	//------------------------------------------------------------------------------------------------
	//! TO DO:
	//! - Rate depending on PalvCO2
	//! - Smoothly adjust RR based on target
	//! - Add opioid depression
	protected float ComputeRespiratoryRate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		if (!context.m_pVitals.CanBreath())
			return 0;
		
		return Math.Min(s_pBreathingSettings.m_fMaxRespiratoryRateBPM, s_pBreathingSettings.m_fDefaultRespiratoryRateBPM / (1 - context.m_pVitals.GetPneumothoraxScale()));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Apply reduction of volume due to tension pneumothorax
	override protected float ComputeStrokeVolume(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float volume = super.ComputeStrokeVolume(context, timeSlice);
		if (context.m_pVitals.HasTensionPneumothorax())
			volume *= s_pBreathingSettings.m_fTensionPneumothoraxStrokeVolumeScale;

		return volume;
	}

	//------------------------------------------------------------------------------------------------
	protected void UpdateOxygenMetabolism(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float hypoxicSpO2 = s_pBreathingSettings.m_fHypoxicMetabolismSpO2;
		float defaultSpO2 = s_pBreathingSettings.m_fDefaultSpO2;
		float lambda = Math.InverseLerp(hypoxicSpO2, defaultSpO2, Math.Clamp(context.m_pVitals.GetSpO2(), hypoxicSpO2, defaultSpO2));
		// Interpolate RER and oxygen consumption based on SpO2. Oxygen consumption at reduced SpO2 is lowered to simulate the body focusing on vital body parts.
		context.m_fOxygenConsumption = Math.Lerp(s_pBreathingSettings.m_fHypoxicOxygenConsumptionMMOLPS, s_pBreathingSettings.m_fDefaultOxygenConsumptionMMOLPS, lambda);
		float rer = Math.Lerp(s_pBreathingSettings.m_fHypoxicRER, s_pBreathingSettings.m_fDefaultRER, lambda);
		context.m_fCO2Production = context.m_fOxygenConsumption * rer;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateVentilation(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float scaledRate = context.m_pVitals.GetRespiratoryRate() / 60.0 *  context.m_pVitals.GetTidalVolume() / context.m_pVitals.GetCapacityVolume();
		
		// Enforce minimum rate when BVM is present
		ACE_EquipmentStorageComponent storageComponent = ACE_EquipmentStorageComponent.Cast(context.m_pObject.FindComponent(ACE_EquipmentStorageComponent));
		if (storageComponent && storageComponent.IsSlotOccupied(ACE_EEquipementSlot.OXYGEN_MASK) && context.m_pVitals.CanBreath())
			scaledRate = Math.Max(0.015, scaledRate);
		
		context.m_fVentFluxO2 = scaledRate * (context.m_pVitals.GetPalvO2Max() - context.m_pVitals.GetPalvO2());
		context.m_fVentFluxCO2 = scaledRate * context.m_pVitals.GetPalvCO2();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdatePerfusion(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float perfusion = 0;
		float pneumothoraxScale = context.m_pVitals.GetPneumothoraxScale();

		if (pneumothoraxScale < 1)
			perfusion = context.m_pVitals.GetCardiacOutput() / 60 * Math.Pow(1 - pneumothoraxScale, 2/3);
		
		if (float.AlmostEqual(0, perfusion))
		{
			context.m_fPerfFluxO2 = 0;
			context.m_fPerfFluxCO2 = 0;
			return;
		}
		
		// Compute O2 flux
		float alveolarOxygenSaturation = ACE_Math.Hill(Math.Pow(context.m_pVitals.GetPalvO2() / ACE_MaterialProperties.DEFAULT_KD_HEMOGLOBIN, ACE_MaterialProperties.HILL_COEFF_HEMOGLOBIN));
		float saturationLoss = 60 / context.m_pVitals.GetCardiacOutput() / context.m_pBloodHitZone.ACE_Medical_GetHemeConcentration() * context.m_fOxygenConsumption;
		float venousOxygenSaturation = Math.Max(0, context.m_pVitals.GetSpO2() / 100 - saturationLoss);
		float equilibriumShift = alveolarOxygenSaturation - venousOxygenSaturation;
		float minChange = -venousOxygenSaturation;
		float maxChange = Math.Max(0, equilibriumShift);
		context.m_fPerfFluxO2 = Math.Clamp(ACE_Medical_VitalsComponent.O2_SATURATION_EQUILIBRATION_RATE_SCALE * equilibriumShift, minChange, maxChange);
		context.m_fPerfFluxO2 *= perfusion * context.m_pBloodHitZone.ACE_Medical_GetHemeConcentration();
		
		// Compute CO2 flux
		equilibriumShift = context.m_pVitals.GetCvenCO2() - ACE_MaterialProperties.KH_CO2_BLOOD * context.m_pVitals.GetPalvCO2();
		minChange = Math.Min(0, equilibriumShift);
		maxChange =  context.m_pVitals.GetCvenCO2() + ACE_Medical_VitalsComponent.BLOOD_HCO3_CONCENTRATION;
		context.m_fPerfFluxCO2 = Math.Clamp(ACE_Medical_VitalsComponent.CARBONATE_DEHYDRATION_RATE_SCALE * equilibriumShift, minChange, maxChange);
		context.m_fPerfFluxCO2 *= perfusion;
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputePalvO2(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return Math.Clamp(context.m_pVitals.GetPalvO2() + timeSlice * (context.m_fVentFluxO2 - ACE_PhysicalConstants.R * context.m_pVitals.GetTemperature() / context.m_pVitals.GetCapacityVolume() * context.m_fPerfFluxO2), 0, context.m_pVitals.GetPalvO2Max());
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeSpO2(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return 100 * Math.Clamp(context.m_pVitals.GetSpO2() / 100 + timeSlice * (context.m_fPerfFluxO2 - context.m_fOxygenConsumption) / context.m_pBloodHitZone.ACE_Medical_GetArterialBloodVolume() / context.m_pBloodHitZone.ACE_Medical_GetHemeConcentration(), 0, 1);
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeCvenCO2(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return context.m_pVitals.GetCvenCO2() + timeSlice * (context.m_fCO2Production - context.m_fPerfFluxCO2) / context.m_pBloodHitZone.ACE_Medical_GetVenousBloodVolume();
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputePalvCO2(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return Math.Max(context.m_pVitals.GetPalvCO2() + timeSlice * (ACE_PhysicalConstants.R * context.m_pVitals.GetTemperature() / context.m_pVitals.GetCapacityVolume() * context.m_fPerfFluxCO2 - context.m_fVentFluxCO2), 0);
	}
}
