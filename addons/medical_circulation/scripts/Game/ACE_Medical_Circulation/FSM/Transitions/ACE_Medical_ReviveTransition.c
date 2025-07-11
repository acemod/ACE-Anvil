//------------------------------------------------------------------------------------------------
class ACE_Medical_ReviveTransition : ACE_FSM_ITransition<ACE_Medical_VitalStates_CharacterContext>
{
	protected static ACE_Medical_Circulation_Settings s_pCirculationSettings;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_ReviveTransition(ACE_FSM_EStateID fromStateIDs, ACE_FSM_EStateID toStateID)
	{
		if (!s_pCirculationSettings)
			s_pCirculationSettings = ACE_SettingsHelperT<ACE_Medical_Circulation_Settings>.GetModSettings();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool ShouldBePerformed(ACE_Medical_VitalStates_CharacterContext context, float timeSlice)
	{
		// Cannot revive when MAP is too high
		if (context.m_pComponent.GetMeanArterialPressure() > s_pCirculationSettings.m_CardiacArrestThresholds.m_fMeanArterialPressureHighKPA)
			return false;
		
		/***** context.m_fCPRSuccessCheckTimerS += timeSlice * context.m_pComponent.GetReviveSuccessCheckTimerScale(); *****/
		
		if (context.m_fCPRSuccessCheckTimerS < s_pCirculationSettings.m_fCPRSuccessCheckTimeoutS)
			return false;
		
		context.m_fCPRSuccessCheckTimerS = 0;
		return (Math.RandomFloat(0, 1) < ComputeReviveChance(context));
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeReviveChance(ACE_Medical_VitalStates_CharacterContext context)
	{
		float minScale = context.m_pBloodHitZone.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_4_HEMORRHAGE);
		float maxScale = context.m_pBloodHitZone.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_2_HEMORRHAGE);
		float scale = Math.Clamp(context.m_pBloodHitZone.GetHealthScaled(), minScale, maxScale);
		return Math.Map(scale, minScale, maxScale, s_pCirculationSettings.m_fCPRSuccessChanceMin, s_pCirculationSettings.m_fCPRSuccessChanceMax);
	}
}
