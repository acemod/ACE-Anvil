//------------------------------------------------------------------------------------------------
class ACE_Medical_ReviveSuccessDrugEffect : ACE_Medical_HillDrugEffectConfig
{
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(ACE_Medical_CharacterContext targetContext, map<ACE_Medical_EDrugType, float> concentrations)
	{
		targetContext.m_pVitals.SetReviveSuccessCheckTimerScale(ComputeEffect(concentrations) + 1);
	}
	
	//------------------------------------------------------------------------------------------------
	override void ClearEffect(ACE_Medical_CharacterContext targetContext)
	{
		targetContext.m_pVitals.SetReviveSuccessCheckTimerScale(1);
	}
	
#ifdef ENABLE_DIAG
	//------------------------------------------------------------------------------------------------
	override void OnDiag(ACE_Medical_CharacterContext targetContext, float timeSlice)
	{
		DbgUI.Text(string.Format("- Revive success scale:    %1", targetContext.m_pVitals.GetReviveSuccessCheckTimerScale()));
	}
#endif
}
