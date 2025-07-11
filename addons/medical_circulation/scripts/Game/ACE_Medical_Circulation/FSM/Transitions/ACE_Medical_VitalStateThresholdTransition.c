//------------------------------------------------------------------------------------------------
class ACE_Medical_VitalStateThresholdTransition : ACE_FSM_ITransition<ACE_Medical_VitalStates_CharacterContext>
{
	protected ref ACE_Medical_VitalStateThresholdsConfig m_pThresholds;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_VitalStateThresholdTransition(ACE_FSM_EStateID fromStateIDs, ACE_FSM_EStateID toStateID, ACE_Medical_VitalStateThresholdsConfig thresholds)
	{
		m_pThresholds = thresholds;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool ShouldBePerformed(ACE_Medical_VitalStates_CharacterContext context, float timeSlice)
	{
		float heartRate = context.m_pComponent.GetHeartRate();
		if ((heartRate < m_pThresholds.m_fHeartRateLowBPM) || (heartRate > m_pThresholds.m_fHeartRateHighBPM))
			return true;
		
		float meanArterialPressure = context.m_pComponent.GetMeanArterialPressure();
		if ((meanArterialPressure < m_pThresholds.m_fMeanArterialPressureLowKPA) && (heartRate <= m_pThresholds.m_fMaxHeartForMeanArterialPressureLowBPM))
			return true;
		
		if (meanArterialPressure > m_pThresholds.m_fMeanArterialPressureHighKPA)
			return true;
		
		if (context.m_pBloodHitZone.GetHealthScaled() <= context.m_pBloodHitZone.GetDamageStateThreshold(m_pThresholds.m_eBloodState))
			return true;
		
		return false;
	}
}
