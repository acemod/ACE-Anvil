//------------------------------------------------------------------------------------------------
class ACE_Medical_ResuscitationState : ACE_Medical_StableState
{
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		CalculateMetabolicAcidosisRecovery(context, timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	//! In resuscitation, acidosis can only decrease (recover)
	//! If blood volume <= 60%, no acidosis change occurs 
	override protected void CalculateMetabolicAcidosisRecovery(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float bloodVolumeRatio = context.m_pBloodHitZone.GetHealthScaled();
		
		// Only allow acidosis to decrease if blood volume > 60%
		if (bloodVolumeRatio > ACIDOSIS_NO_CHANGE_THRESHOLD)
			UpdateMetabolicAcidosis(context, timeSlice, false);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Fake heart rate due to CPR
	override protected float ComputeHeartRate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return 100 + Math.Clamp(Math.AbsFloat(Math.RandomGaussFloat(20/3, 0)), 0, 20);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected float ComputeStrokeVolume(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return s_pCirculationSettings.m_fCPRStrokeVolumeScale * super.ComputeStrokeVolume(context, timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Prevent BP from getting absurdly high through medication
	override protected float ComputeMeanArterialPressure(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return Math.Min(super.ComputeMeanArterialPressure(context, timeSlice), s_pCirculationSettings.m_CardiacArrestThresholds.m_fMeanArterialPressureHighKPA);
	}
}
