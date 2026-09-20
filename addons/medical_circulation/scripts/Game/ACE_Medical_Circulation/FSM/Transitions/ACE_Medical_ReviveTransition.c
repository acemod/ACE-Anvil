//------------------------------------------------------------------------------------------------
class ACE_Medical_ReviveTransition : ACE_FSM_ITransition<ACE_Medical_CharacterContext>
{
	protected static ACE_Medical_Circulation_Settings s_pCirculationSettings;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_ReviveTransition(ACE_FSM_EStateID fromStateIDs, ACE_FSM_EStateID toStateID)
	{
		if (!s_pCirculationSettings)
			s_pCirculationSettings = ACE_SettingsHelperT<ACE_Medical_Circulation_Settings>.GetModSettings();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPerform(ACE_Medical_CharacterContext context)
	{
		context.m_pVitals.OnRevived();
		context.m_pVitals.SetHeartRate(s_pCirculationSettings.m_CriticalThresholds.m_fHeartRateLowBPM);
		// Add regneration when no longer in cardiac arrest
		context.m_pDamageManager.RegenVirtualHitZone(context.m_pDamageManager.ACE_Medical_GetBrainHitZone());
	}
	
	//------------------------------------------------------------------------------------------------
	override bool ShouldBePerformed(ACE_Medical_CharacterContext context, float timeSlice)
	{
		// Cannot revive when MAP is too high
		if (context.m_pVitals.GetMeanArterialPressure() > s_pCirculationSettings.m_CardiacArrestThresholds.m_fMeanArterialPressureHighKPA)
			return false;
		
		context.m_fCPRSuccessCheckTimerS += timeSlice * context.m_pVitals.GetReviveSuccessCheckTimerScale();
		
		if (context.m_fCPRSuccessCheckTimerS < s_pCirculationSettings.m_fCPRSuccessCheckTimeoutS)
			return false;
		
		context.m_fCPRSuccessCheckTimerS = 0;
		return (Math.RandomFloat(0, 1) < ComputeReviveChance(context));
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeReviveChance(ACE_Medical_CharacterContext context)
	{
		float minScale = context.m_pBloodHitZone.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_4_HEMORRHAGE);
		float maxScale = context.m_pBloodHitZone.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_2_HEMORRHAGE);
		float scale = Math.Clamp(context.m_pBloodHitZone.GetHealthScaled(), minScale, maxScale);
		return Math.Map(scale, minScale, maxScale, s_pCirculationSettings.m_fCPRSuccessChanceMin, s_pCirculationSettings.m_fCPRSuccessChanceMax);
	}
}
