modded class ACE_Medical_CardiacArrestState
{
	//------------------------------------------------------------------------------------------------
	override protected ACE_Medical_ECardiacRhythm ComputeCardiacRhythm(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float hemIV = context.m_pBloodHitZone.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_4_HEMORRHAGE);
		float hemII = context.m_pBloodHitZone.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_2_HEMORRHAGE);
		float currentHealthScaled = context.m_pBloodHitZone.GetHealthScaled();

		if (context.m_pVitals.IsCPRPerformed())
			return ACE_Medical_ECardiacRhythm.Unknown;

		if (currentHealthScaled <= hemIV)
		{
			return ACE_Medical_ECardiacRhythm.PEA;
		}
		else
		{
			return ACE_Medical_ECardiacRhythm.VF;
		}

		return ACE_Medical_ECardiacRhythm.Unknown;
	}
}