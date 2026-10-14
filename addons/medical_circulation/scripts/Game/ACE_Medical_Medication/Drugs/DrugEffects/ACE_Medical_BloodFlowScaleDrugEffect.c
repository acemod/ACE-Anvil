//------------------------------------------------------------------------------------------------
class ACE_Medical_BloodFlowScaleDrugEffect : ACE_Medical_HillDrugEffectConfig
{
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(ACE_Medical_CharacterContext targetContext, map<ACE_Medical_EDrugType, float> concentrations, float timeSlice)
	{
		targetContext.m_pDamageManager.ACE_Medical_SetBloodFlowScale(1 - ComputeEffect(concentrations));
	}
	
	//------------------------------------------------------------------------------------------------
	override void ClearEffect(ACE_Medical_CharacterContext targetContext)
	{
		targetContext.m_pDamageManager.ACE_Medical_SetBloodFlowScale(1);
	}
	
#ifdef ENABLE_DIAG
	//------------------------------------------------------------------------------------------------
	override void OnDiag(ACE_Medical_CharacterContext targetContext, float timeSlice)
	{
		DbgUI.Text(string.Format("- Blood flow scale:        %1", targetContext.m_pDamageManager.ACE_Medical_GetBloodFlowScale()));
	}
#endif
}
