//------------------------------------------------------------------------------------------------
modded class ACE_Medical_CardiacArrestState : ACE_Medical_IVitalState
{
	//------------------------------------------------------------------------------------------------
	override protected float ComputeRespiratoryRate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return 0;
	}
}
