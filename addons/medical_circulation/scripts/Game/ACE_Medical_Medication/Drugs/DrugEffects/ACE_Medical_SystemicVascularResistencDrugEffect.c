//------------------------------------------------------------------------------------------------
class ACE_Medical_SystemicVascularResistencDrugEffect : ACE_Medical_LinearDrugEffectConfig
{
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(ACE_Medical_CharacterContext targetContext, map<ACE_Medical_EDrugType, float> concentrations)
	{
		targetContext.m_pVitals.SetSystemicVascularResistenceMedicationAdjustment(ComputeEffect(concentrations));
	}
	
	//------------------------------------------------------------------------------------------------
	override void ClearEffect(ACE_Medical_CharacterContext targetContext)
	{
		targetContext.m_pVitals.SetSystemicVascularResistenceMedicationAdjustment(0);
	}
	
#ifdef ENABLE_DIAG
	//------------------------------------------------------------------------------------------------
	override void OnDiag(ACE_Medical_CharacterContext targetContext, float timeSlice)
	{
		DbgUI.Text(string.Format("- SVR:                     %1", targetContext.m_pVitals.GetSystemicVascularResistenceMedicationAdjustment()));
	}
#endif
}
