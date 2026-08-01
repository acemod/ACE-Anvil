//------------------------------------------------------------------------------------------------
class ACE_Medical_HeartRateEffect : ACE_Medical_LinearDrugEffectConfig
{
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(ACE_Medical_CharacterContext targetContext, map<ACE_Medical_EDrugType, float> concentrations, float timeSlice)
	{
		targetContext.m_pVitals.SetHeartRateMedicationAdjustment(ComputeEffect(concentrations));
	}
	
	//------------------------------------------------------------------------------------------------
	override void ClearEffect(ACE_Medical_CharacterContext targetContext)
	{
		targetContext.m_pVitals.SetHeartRateMedicationAdjustment(0);
	}
	
#ifdef ENABLE_DIAG
	//------------------------------------------------------------------------------------------------
	override void OnDiag(ACE_Medical_CharacterContext targetContext, float timeSlice)
	{
		DbgUI.Text(string.Format("- HR:                      %1 bpm", targetContext.m_pVitals.GetHeartRateMedicationAdjustment()));
	}
#endif
}
