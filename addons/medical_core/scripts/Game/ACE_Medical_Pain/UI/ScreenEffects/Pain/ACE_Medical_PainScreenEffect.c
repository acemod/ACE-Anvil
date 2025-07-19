//------------------------------------------------------------------------------------------------
//! Pain screen effect; Code is based on SCR_BleedingScreenEffect
class ACE_Medical_PainScreenEffect : SCR_BaseScreenEffect
{
	[Attribute()]
	protected ref array<ref ACE_Medical_BasePainScreenEffectMode> m_aModes;
	
	protected ACE_Medical_BasePainScreenEffectMode m_pSelectedMode;
	protected SCR_CharacterControllerComponent m_pCharController;
	protected SCR_CharacterDamageManagerComponent m_pDamageManager;
	protected ACE_Medical_PainHitZone m_pPainHitZone;
	
	//------------------------------------------------------------------------------------------------
	override void DisplayControlledEntityChanged(IEntity from, IEntity to)
	{
		if (!m_pSelectedMode)
			return;
		
		ClearEffects();

		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(to);
		if (!char)
			return;
		
		m_pSelectedMode.InitEffect(char, m_wRoot);
		
		m_pCharController = SCR_CharacterControllerComponent.Cast(char.GetCharacterController());
		if (!m_pCharController)
			return;
		
		m_pDamageManager = SCR_CharacterDamageManagerComponent.Cast(char.GetDamageManager());
		if (!m_pDamageManager)
			return;
		
		m_pPainHitZone = m_pDamageManager.ACE_Medical_GetPainHitZone();
		if (!m_pPainHitZone)
			return;
		
		UpdateEnablePainEffect();
		m_pPainHitZone.GetOnDamageStateChanged().Insert(OnPainStateChanged);
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
		if (!m_pSelectedMode)
			return;
		
		if (m_pSelectedMode.ShouldGetUpdated())
			m_pSelectedMode.UpdateEffect(timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Clear pain effect over time
	override void ClearEffects()
	{
		if (m_pSelectedMode)
			m_pSelectedMode.EnableEffect(false);
		
		if (m_pPainHitZone)
			m_pPainHitZone.GetOnDamageStateChanged().Remove(OnPainStateChanged);
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void DisplayOnSuspended()
	{
		if (m_pSelectedMode)
			m_pSelectedMode.EnableEffect(false);
	}

	//------------------------------------------------------------------------------------------------
	protected override void DisplayOnResumed()
	{
		UpdateEnablePainEffect();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateEnablePainEffect()
	{
		if (!m_pSelectedMode)
			return;
		
		if (m_pDamageManager.ACE_Medical_IsInPain() && m_pCharController.GetLifeState() == ECharacterLifeState.ALIVE)
			m_pSelectedMode.EnableEffect(true);
		else
			m_pSelectedMode.TerminateEffect();
	}
	
	//------------------------------------------------------------------------------------------------
	void SetPainEffectType(ACE_Medical_EPainEffectType type)
	{
		foreach (ACE_Medical_BasePainScreenEffectMode mode : m_aModes)
		{
			if (mode.GetEffectType() != type)
				continue;
			
			m_pSelectedMode = mode;
			return;
		}
	}
}
