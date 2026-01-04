modded class ACE_Medical_IVitalState
{
	//------------------------------------------------------------------------------------------------
	override void OnUpdate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		context.m_pVitals.SetCardiacRhythm(ComputeCardiacRhythm(context, timeSlice));
	}
	
	//------------------------------------------------------------------------------------------------
	protected ACE_Medical_ECardiacRhythm ComputeCardiacRhythm(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float hr = context.m_pVitals.GetHeartRate();

		if (context.m_pVitals.IsCPRPerformed())
			return ACE_Medical_ECardiacRhythm.Unknown;

		if (hr >= 0 && hr < 60)
			return ACE_Medical_ECardiacRhythm.Bradycardia;

		if (hr >= 60 && hr <= 100)
			return ACE_Medical_ECardiacRhythm.Sinus;

		if (hr > 100)
			return ACE_Medical_ECardiacRhythm.Tachycardia;

		return ACE_Medical_ECardiacRhythm.Unknown;
	}
}