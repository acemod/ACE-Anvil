//------------------------------------------------------------------------------------------------
class ACE_Medical_FluidReplacementDrugEffect : ACE_Medical_DrugEffectConfig
{
	[Attribute(defvalue: "1", desc: "Blood volume restored per second for a concentration of 1 [ml/s]", params: "0 inf")]
	protected float m_fMlPerConcentrationPerSecond;
	
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(ACE_Medical_CharacterContext targetContext, map<ACE_Medical_EDrugType, float> concentrations, float timeSlice)
	{
		if (timeSlice <= 0)
			return;
		
		SCR_CharacterBloodHitZone bloodHitZone = targetContext.m_pBloodHitZone;
		if (!bloodHitZone)
			return;
	
		float concentration = ComputeEffect(concentrations);
		if (concentration <= 0)
			return;
	
		float mlDelta = concentration * m_fMlPerConcentrationPerSecond * timeSlice;
		float newHealth = Math.Min(bloodHitZone.GetMaxHealth(), bloodHitZone.GetHealth() + mlDelta);
		bloodHitZone.SetHealth(newHealth);
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
