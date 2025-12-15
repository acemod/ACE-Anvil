//------------------------------------------------------------------------------------------------
modded class ACE_Medical_IVitalState : ACE_FSM_IState<ACE_Medical_CharacterContext>
{
	protected static ACE_Medical_Breathing_Settings s_pBreathingSettings;
	protected static float s_fDefaultPerfusion;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_IVitalState(ACE_FSM_EStateID id)
	{
		if (!s_pBreathingSettings)
		{
			s_pBreathingSettings = ACE_SettingsHelperT<ACE_Medical_Breathing_Settings>.GetModSettings();
			s_fDefaultPerfusion = s_pCirculationSettings.m_fDefaultStrokeVolumeML * s_pCirculationSettings.m_fDefaultHeartRateBPM / 60;
		}
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
	//! TO DO: Rate depending on PalvCO2
	protected float ComputeRespiratoryRate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		if (!context.m_pVitals.CanBreath())
			return 0;
		
		return Math.Min(s_pBreathingSettings.m_fMaxRespiratoryRateBPM, s_pBreathingSettings.m_fDefaultRespiratoryRateBPM / (1 - context.m_pVitals.GetPneumothoraxScale()));
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateOxygenMetabolism(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float lambda = Math.InverseLerp(75, 97, Math.Clamp(context.m_pVitals.GetSpO2(), 75, 97));
		context.m_fOxygenConsumption = Math.Lerp(0.013, 0.208, lambda);
		context.m_fCO2Production = Math.Lerp(0.0156, 0.1664, lambda);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateVentilation(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float scaledRate = context.m_pVitals.GetRespiratoryRate() / 60 *  context.m_pVitals.GetTidalVolume() / context.m_pVitals.GetCapacityVolume();
		context.m_fVentFluxO2 = scaledRate * (ACE_Medical_VitalsComponent.PALVO2_MAX - context.m_pVitals.GetPalvO2());
		context.m_fVentFluxCO2 = scaledRate * context.m_pVitals.GetPalvCO2();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdatePerfusion(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float alveolarOxygenSaturation = ACE_Math.Hill(Math.Pow(context.m_pVitals.GetPalvO2() / ACE_MaterialProperties.DEFAULT_KD_HEMOGLOBIN, ACE_MaterialProperties.HILL_COEFF_HEMOGLOBIN));
		float perfusion = context.m_pVitals.GetCardiacOutput() / 60 * Math.Pow(1 - context.m_pVitals.GetPneumothoraxScale(), 2/3);
		float maxFluxO2 = perfusion * context.m_pBloodHitZone.ACE_Medical_GetHemeConcentration() * alveolarOxygenSaturation;
		
		float rateScale = perfusion / s_fDefaultPerfusion;
		if (!context.m_pVitals.CanBreath())
			rateScale = Math.Min(rateScale, 1/22);
		
		context.m_fPerfFluxO2 = Math.Clamp((Math.Min(rateScale, 1) * context.m_fOxygenConsumption + rateScale * (alveolarOxygenSaturation - context.m_pVitals.GetSpO2() / 100)), -maxFluxO2, maxFluxO2);
		
		float equilibriumShift = context.m_pVitals.GetCvenCO2() - ACE_MaterialProperties.KH_CO2_BLOOD * context.m_pVitals.GetPalvCO2();
		float maxEquilibriumFluxScale = 1 / (1 / context.m_pBloodHitZone.ACE_Medical_GetVenousBloodVolume() + ACE_PhysicalConstants.R * context.m_pVitals.GetTemperature() * ACE_MaterialProperties.KH_CO2_BLOOD / context.m_pVitals.GetCapacityVolume());
		context.m_fPerfFluxCO2 = equilibriumShift * Math.Clamp(perfusion * ACE_Medical_VitalsComponent.CARBONATE_DEHYDRATION_RATE_SCALE, 0, maxEquilibriumFluxScale);
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputePalvO2(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return Math.Clamp(context.m_pVitals.GetPalvO2() + timeSlice * (context.m_fVentFluxO2 - ACE_PhysicalConstants.R * context.m_pVitals.GetTemperature() / context.m_pVitals.GetCapacityVolume() * context.m_fPerfFluxO2), 0, ACE_Medical_VitalsComponent.PALVO2_MAX);
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
