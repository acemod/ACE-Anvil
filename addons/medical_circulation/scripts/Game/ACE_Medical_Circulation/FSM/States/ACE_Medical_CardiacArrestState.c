//------------------------------------------------------------------------------------------------
class ACE_Medical_CardiacArrestState : ACE_Medical_IVitalState
{
	//------------------------------------------------------------------------------------------------
	override void OnEnter(ACE_Medical_VitalStates_CharacterContext context)
	{
		super.OnEnter(context);
		context.m_bWasInCardiacArrest = true;
		context.m_pDamageManager.AddDamageEffect(s_pCirculationSettings.m_CardiacArrestDamageEffect);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnExit(ACE_Medical_VitalStates_CharacterContext context)
	{
		super.OnExit(context);
		context.m_pDamageManager.TerminateDamageEffectsOfType(s_pCirculationSettings.m_CardiacArrestDamageEffect.Type());
	}
	
	//------------------------------------------------------------------------------------------------
	override protected float ComputeHeartRate(ACE_Medical_VitalStates_CharacterContext context, float timeSlice)
	{
		return 0;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected float ComputeCardiacOutput(ACE_Medical_VitalStates_CharacterContext context, float timeSlice)
	{
		return 0;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected float ComputeSystemicVascularResistance(ACE_Medical_VitalStates_CharacterContext context, float timeSlice)
	{
		return 0;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected float ComputeMeanArterialPressure(ACE_Medical_VitalStates_CharacterContext context, float timeSlice)
	{
		return 0;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected float ComputePulsePressure(ACE_Medical_VitalStates_CharacterContext context, float timeSlice)
	{
		return 0;
	}
}
