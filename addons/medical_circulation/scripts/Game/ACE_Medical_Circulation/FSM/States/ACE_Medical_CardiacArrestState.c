//------------------------------------------------------------------------------------------------
class ACE_Medical_CardiacArrestState : ACE_Medical_StableState
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
	override void OnUpdate(ACE_Medical_CharacterContext context, float timeSlice)
	{
		super.OnUpdate(context, timeSlice);
		CalculateMetabolicAcidosisBuildup(context, timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	//! In cardiac arrest, acidosis can only increase (worsen)
	//! If blood volume > 60%, no acidosis change occurs
	protected void CalculateMetabolicAcidosisBuildup(ACE_Medical_CharacterContext context, float timeSlice)
	{
		float bloodVolumeRatio = context.m_pBloodHitZone.GetHealthScaled();
		
		// No acidosis change if blood volume > 60%
		if (bloodVolumeRatio > ACIDOSIS_NO_CHANGE_THRESHOLD)
			return;
		
		UpdateMetabolicAcidosis(context, timeSlice, true);
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
