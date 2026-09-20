//------------------------------------------------------------------------------------------------
//! Negates threshold check to evaluate the opposite range
class ACE_Medical_VitalStateInverseThresholdTransition : ACE_Medical_VitalStateThresholdTransition
{
	//------------------------------------------------------------------------------------------------
	override bool ShouldBePerformed(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return !super.ShouldBePerformed(context, timeSlice);
	}
}
