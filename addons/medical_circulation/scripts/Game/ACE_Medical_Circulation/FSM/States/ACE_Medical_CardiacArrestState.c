//------------------------------------------------------------------------------------------------
class ACE_Medical_CardiacArrestState : ACE_Medical_IVitalState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_CharacterContext context)
	{
		super.OnEnter(context);
		
		//! Kill AI when cardiac arrest is not allowed
		if (!s_pCirculationSettings.m_bCardiacArrestForAIEnabled && !EntityUtils.IsPlayer(context.m_pObject))
		{
			context.m_pDamageManager.Kill(context.m_pDamageManager.GetInstigator());
			return;
		}
		
		// Force uncon
		context.m_pDamageManager.GetResilienceHitZone().SetHealth(0);
		context.m_pDamageManager.UpdateConsciousness();
		
		context.m_pDamageManager.AddDamageEffect(s_pCirculationSettings.m_CardiacArrestDamageEffect);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnExit(ACE_Medical_CharacterContext context)
	{
		super.OnExit(context);
		context.m_pDamageManager.TerminateDamageEffectsOfType(s_pCirculationSettings.m_CardiacArrestDamageEffect.Type());
	}
	
	//------------------------------------------------------------------------------------------------
	override protected float ComputeHeartRate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return 0;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected float ComputeCardiacOutput(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return 0;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected float ComputeSystemicVascularResistance(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return 0;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected float ComputeMeanArterialPressure(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return 0;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected float ComputePulsePressure(ACE_Medical_CharacterContext context, float timeSlice)
	{
		return 0;
	}
	
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
