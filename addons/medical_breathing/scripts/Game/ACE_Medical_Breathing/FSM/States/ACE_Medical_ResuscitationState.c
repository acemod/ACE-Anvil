//------------------------------------------------------------------------------------------------
modded class ACE_Medical_ResuscitationState : ACE_Medical_IVitalState
{
	//------------------------------------------------------------------------------------------------
	override protected float ComputeRespiratoryRate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return 0.3;
	}
}
