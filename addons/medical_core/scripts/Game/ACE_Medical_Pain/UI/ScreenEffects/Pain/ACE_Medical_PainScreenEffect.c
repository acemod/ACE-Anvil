//------------------------------------------------------------------------------------------------
//! Pain screen effect; Code is based on SCR_BleedingScreenEffect
class ACE_Medical_PainScreenEffect : SCR_BaseScreenEffect
{
	[Attribute()]
	protected ref array<ref ACE_Medical_BasePainScreenEffectMode> m_aModes;
	
	protected ACE_Medical_BasePainScreenEffectMode m_SelectedMode;
	protected SCR_CharacterControllerComponent m_CharController;
	protected SCR_CharacterDamageManagerComponent m_DamageManager;
	protected ACE_Medical_PainHitZone m_PainHitZone;
	
	//------------------------------------------------------------------------------------------------
	override void DisplayControlledEntityChanged(IEntity from, IEntity to)
	{
		if (!m_SelectedMode)
			return;
		
		ClearEffects();

		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(to);
		if (!char)
			return;
		
		m_SelectedMode.InitEffect(char, m_wRoot);
		
		m_CharController = SCR_CharacterControllerComponent.Cast(char.GetCharacterController());
		if (!m_CharController)
			return;
		
		m_DamageManager = SCR_CharacterDamageManagerComponent.Cast(char.GetDamageManager());
		if (!m_DamageManager)
			return;
		
		m_PainHitZone = m_DamageManager.ACE_Medical_GetPainHitZone();
		if (!m_PainHitZone)
			return;
		
		UpdateEnablePainEffect();
		m_PainHitZone.GetOnDamageStateChanged().Insert(OnPainStateChanged);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void DisplayConsciousnessChanged(bool conscious, bool init = false)
	{
		if (init)
			return;
		
		UpdateEnablePainEffect();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnPainStateChanged(SCR_HitZone hitZone)
	{
		UpdateEnablePainEffect();
	}
	
	//------------------------------------------------------------------------------------------------
	override void UpdateEffect(float timeSlice)
	{
		if (!m_SelectedMode)
			return;
		
		if (m_SelectedMode.ShouldGetUpdated())
			m_SelectedMode.UpdateEffect(timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clear pain effect over time
	override void ClearEffects()
	{
		if (m_SelectedMode)
			m_SelectedMode.EnableEffect(false);
		
		if (m_PainHitZone)
			m_PainHitZone.GetOnDamageStateChanged().Remove(OnPainStateChanged);
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void DisplayOnSuspended()
	{
		if (m_SelectedMode)
			m_SelectedMode.EnableEffect(false);
	}

	//------------------------------------------------------------------------------------------------
	protected override void DisplayOnResumed()
	{
		UpdateEnablePainEffect();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateEnablePainEffect()
	{
		if (!m_SelectedMode)
			return;
		
		if (m_DamageManager.ACE_Medical_IsInPain() && m_CharController.GetLifeState() == ECharacterLifeState.ALIVE)
			m_SelectedMode.EnableEffect(true);
		else
			m_SelectedMode.TerminateEffect();
	}
	
	//------------------------------------------------------------------------------------------------
	void SetPainEffectType(ACE_Medical_EPainEffectType type)
	{
		foreach (ACE_Medical_BasePainScreenEffectMode mode : m_aModes)
		{
			if (mode.GetEffectType() != type)
				continue;
			
			m_SelectedMode = mode;
			return;
		}
	}
}
