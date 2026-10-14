//------------------------------------------------------------------------------------------------
class ACE_Medical_FluidReplacementDrugEffect : ACE_Medical_LinearDrugEffectConfig
{
	[Attribute(defvalue: "1", desc: "Blood volume restored per second for a concentration of 1 [ml/s]", params: "0 inf")]
	protected float m_fMlPerConcentrationPerSecond;
	
	protected static ACE_Medical_PharmacokineticsConfig s_pSalineKinetics;
	
	//------------------------------------------------------------------------------------------------
	protected ACE_Medical_PharmacokineticsConfig GetSalineKinetics()
	{
		if (!s_pSalineKinetics)
		{
			ACE_Medical_Medication_Settings settings = ACE_SettingsHelperT<ACE_Medical_Medication_Settings>.GetModSettings();
			if (!settings)
				return null;
			
			s_pSalineKinetics = settings.GetPharmacokineticsConfig(ACE_Medical_EDrugType.SALINE);
		}
		
		return s_pSalineKinetics;
	}
	
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(ACE_Medical_CharacterContext targetContext, map<ACE_Medical_EDrugType, float> concentrations, float timeSlice)
	{
		if (timeSlice <= 0)
			return;
		
		SCR_CharacterBloodHitZone bloodHitZone = targetContext.m_pBloodHitZone;
		if (!bloodHitZone)
			return;
	
		ACE_Medical_PharmacokineticsConfig salineKinetics = GetSalineKinetics();
		if (!salineKinetics)
			return;
	
		float concentration = ComputeEffect(concentrations);
		if (concentration <= 0)
			return;
	
		float eliminationRate = salineKinetics.m_fDeactivationRateConstant;
		float infusionRate = concentration * eliminationRate * m_fMlPerConcentrationPerSecond;
		float maxHealth = bloodHitZone.GetMaxHealth();
		float mlDelta = infusionRate * timeSlice;
		float newHealth = Math.Min(maxHealth, bloodHitZone.GetHealth() + mlDelta);
		bloodHitZone.SetHealth(newHealth);
		
		if (Math.AbsFloat(newHealth - maxHealth) <= 0.001)
		{
			ACE_Medical_MedicationComponent medication = targetContext.m_pMedication;
			if (medication)
				medication.StopInfusionsForDrug(ACE_Medical_EDrugType.SALINE);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void ClearEffect(ACE_Medical_CharacterContext targetContext)
	{
		// No persistent state to clear
	}
	
#ifdef ENABLE_DIAG
	//------------------------------------------------------------------------------------------------
	override void OnDiag(ACE_Medical_CharacterContext targetContext, float timeSlice)
	{
		SCR_CharacterBloodHitZone bloodHitZone = targetContext.m_pBloodHitZone;
		if (!bloodHitZone)
			return;
		
		DbgUI.Text(string.Format("- Blood volume:           %1 / %2 ml", bloodHitZone.GetHealth(), bloodHitZone.GetMaxHealth()));
	}
#endif
}
