modded class ACE_Medical_CardiacArrestState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_CharacterContext context)
	{
		super.OnEnter(context);
		
		context.m_pVitals.ModifyTimesArrested(1);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected ACE_Medical_Defibrillation_ECardiacRhythm ComputeCardiacRhythm(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float hemIV = context.m_pBloodHitZone.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_4_HEMORRHAGE);
		float hemII = context.m_pBloodHitZone.GetDamageStateThreshold(ACE_Medical_EBloodState.CLASS_2_HEMORRHAGE);
		float currentHealthScaled = context.m_pBloodHitZone.GetHealthScaled();

		if (context.m_pVitals.IsCPRPerformed())
			return ACE_Medical_Defibrillation_ECardiacRhythm.Unknown;

		if (currentHealthScaled <= hemIV)
		{
			return ACE_Medical_Defibrillation_ECardiacRhythm.PEA;
		}
		else
		{
			return ACE_Medical_Defibrillation_ECardiacRhythm.VF;
		}

		return ACE_Medical_Defibrillation_ECardiacRhythm.Unknown;
	}
}