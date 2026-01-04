modded class ACE_Medical_ResuscitationState
{
	//------------------------------------------------------------------------------------------------
	override protected ACE_Medical_Defibrillation_ECardiacRhythm ComputeCardiacRhythm(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return ACE_Medical_Defibrillation_ECardiacRhythm.Unknown;
	}
}