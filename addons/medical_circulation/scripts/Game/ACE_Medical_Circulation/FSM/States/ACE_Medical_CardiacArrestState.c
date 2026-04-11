//------------------------------------------------------------------------------------------------
class ACE_Medical_CardiacArrestState : ACE_Medical_IVitalState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_CharacterContext context)
	{
		super.OnEnter(context);
		
		//! Kill AI when cardiac arrest is not allowed
		if (!s_pCirculationSettings.m_bCardiacArrestForAIEnabled && !ACE_EntityUtils.IsPlayer(context.m_pObject))
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
}
