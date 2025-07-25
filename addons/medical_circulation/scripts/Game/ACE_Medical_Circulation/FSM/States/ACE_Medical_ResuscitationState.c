//------------------------------------------------------------------------------------------------
class ACE_Medical_ResuscitationState : ACE_Medical_IVitalState
{
	//------------------------------------------------------------------------------------------------
	//! Fake heart rate due to CPR
	override protected float ComputeHeartRate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return 100 + Math.Clamp(Math.AbsFloat(Math.RandomGaussFloat(20/3, 0)), 0, 20);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Prevent BP from getting absurdly high through medication
	override protected float ComputeMeanArterialPressure(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return Math.Min(super.ComputeMeanArterialPressure(context, timeSlice), s_pCirculationSettings.m_CardiacArrestThresholds.m_fMeanArterialPressureHighKPA);
	}
}
