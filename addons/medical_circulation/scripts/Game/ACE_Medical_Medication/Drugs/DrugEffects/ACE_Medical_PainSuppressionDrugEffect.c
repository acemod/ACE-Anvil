//------------------------------------------------------------------------------------------------
class ACE_Medical_PainSuppressionDrugEffect : ACE_Medical_HillDrugEffectConfig
{
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(ACE_Medical_CharacterContext targetContext, map<ACE_Medical_EDrugType, float> concentrations)
	{
		targetContext.m_pDamageManager.ACE_Medical_SetPainSuppression(ComputeEffect(concentrations));
	}
	
	//------------------------------------------------------------------------------------------------
	override void ClearEffect(ACE_Medical_CharacterContext targetContext)
	{
		targetContext.m_pDamageManager.ACE_Medical_SetPainSuppression(0);
	}
	
#ifdef ENABLE_DIAG
	//------------------------------------------------------------------------------------------------
	override void OnDiag(ACE_Medical_CharacterContext targetContext, float timeSlice)
	{
		DbgUI.Text(string.Format("- Pain suppression:        %1 %", targetContext.m_pDamageManager.ACE_Medical_GetPainSuppression()));
	}
#endif
}
