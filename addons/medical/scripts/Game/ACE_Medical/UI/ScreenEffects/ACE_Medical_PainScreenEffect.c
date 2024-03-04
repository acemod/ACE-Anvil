//------------------------------------------------------------------------------------------------
//! Pain screen effect; Code is based on SCR_BleedingScreenEffect
class ACE_Medical_PainScreenEffect : SCR_BaseScreenEffect
{
	// Play Animation of CreateEffectOverTime()
	protected const int   PAIN_REPEAT_DELAY 						= 1000;
	protected const float PAINEFFECT_OPACITY_FADEOUT_1_DURATION 	= 0.2;
	protected const float PAINEFFECT_PROGRESSION_FADEOUT_1_DURATION = 0.2;
	protected const float PAINEFFECT_OPACITY_FADEOUT_2_DURATION 	= 0.3;
	protected const float PAINEFFECT_PROGRESSION_FADEOUT_2_DURATION = 3;
	protected const float PAINEFFECT_OPACITY_FADEIN_1_DURATION		= 1;
	protected const float PAINEFFECT_PROGRESSION_FADEIN_1_DURATION  = 4.5;
	
	// Play Animation of BlackoutEffect()
	protected const float BLACKOUT_OPACITY_MULTIPLIER = 0.20;
	
	//Saturation
	private static float s_fSaturation;
		
	// Widgets
	protected ImageWidget m_wWhiteFlash;
	
	// Character
	protected SCR_CharacterDamageManagerComponent m_pDamageManager;
	protected SCR_CharacterControllerComponent m_pCharacterCtrl;
	protected ACE_Medical_PainHitZone m_pPainHZ;
	protected ChimeraCharacter m_pCharacterEntity;

	protected bool m_bHasPainEffect;
	protected bool m_bIsInPain;

	
	//------------------------------------------------------------------------------------------------
	//! Get effects wiget when starting to draw
	override void DisplayStartDraw(IEntity owner)
	{
		m_wWhiteFlash = ImageWidget.Cast(m_wRoot.FindAnyWidget("ACE_Medical_WhiteFlash"));
	}	
	
	//------------------------------------------------------------------------------------------------
	//! Clear effect and reassign event handlers when controlled entity was changed
 	override void DisplayControlledEntityChanged(IEntity from, IEntity to)
	{
		ClearEffects();
		
		m_pCharacterEntity = ChimeraCharacter.Cast(to);
		if (!m_pCharacterEntity)
			return;
		
		m_pDamageManager = SCR_CharacterDamageManagerComponent.Cast(m_pCharacterEntity.GetDamageManager());
		if (!m_pDamageManager)
			return;
		
		m_pCharacterCtrl = SCR_CharacterControllerComponent.Cast(m_pCharacterEntity.FindComponent(SCR_CharacterControllerComponent));
		if (!m_pCharacterCtrl)
			return;

		// define hitzones for later getting
		m_pPainHZ = m_pDamageManager.ACE_Medical_GetPainHitZone();
		
		// Keep track of changes in pain
		m_pPainHZ.GetOnDamageStateChanged().Insert(OnPainStateChanged);
		
		// In case player started PAIN before invokers were established, check if already PAIN
		if (m_pDamageManager.ACE_Medical_IsInPain())
			OnPainStateChanged(m_pPainHZ);
	}

	//------------------------------------------------------------------------------------------------
	//! Add/Remove effect when pain state has changed
	protected void OnPainStateChanged(SCR_HitZone hitZone)
	{
		m_bIsInPain = m_pDamageManager.ACE_Medical_IsInPain();
		if (m_bHasPainEffect)
		{
			if (!m_bIsInPain || m_pDamageManager.GetState() == EDamageState.DESTROYED)
			{
				ClearEffectOverTime();
				GetGame().GetCallqueue().Remove(CreateEffectOverTime);
				GetGame().GetCallqueue().Remove(ClearEffectOverTime);
				m_bHasPainEffect = false;
			};
		}
		else
		{
			if (m_bIsInPain && m_pDamageManager.GetState() != EDamageState.DESTROYED)
			{
				CreateEffectOverTime();
				m_bHasPainEffect = m_bIsInPain;
			};
		};
	}
	
	//------------------------------------------------------------------------------------------------
	//! Fade in (isFadeIn is true) or out of white flash effects
	//! Alternates with ClearEffectOverTime
	protected void CreateEffectOverTime(bool isFadeIn = true)
	{
		if (!m_wWhiteFlash || !m_pPainHZ)
			return;
		
		float effectStrength = 0;
		
		// Pain can only be fealt when conscious
		if (!m_pCharacterCtrl.IsUnconscious())
			effectStrength = m_pDamageManager.ACE_Medical_GetPainIntensity();
				
		m_wWhiteFlash.SetSaturation(1);
		
		if (isFadeIn)
		{
			AnimateWidget.Opacity(m_wWhiteFlash, effectStrength, PAINEFFECT_OPACITY_FADEIN_1_DURATION);
			AnimateWidget.AlphaMask(m_wWhiteFlash, effectStrength * 0.5, PAINEFFECT_PROGRESSION_FADEIN_1_DURATION);
		}
		else
		{
			AnimateWidget.Opacity(m_wWhiteFlash, 0, PAINEFFECT_OPACITY_FADEOUT_2_DURATION);
			AnimateWidget.AlphaMask(m_wWhiteFlash, 0, PAINEFFECT_PROGRESSION_FADEOUT_2_DURATION);
		}

		GetGame().GetCallqueue().CallLater(ClearEffectOverTime, 1000, false, true, isFadeIn);
	}

	//------------------------------------------------------------------------------------------------
	//! Fade out pain effect.
	//! Ends white flash effect if repeat is false
	//! If repeat is true, then alternates with CreateEffectOverTime and toggles isFadeIn
	protected void ClearEffectOverTime(bool repeat = false, bool isFadeIn = false)
	{
		AnimateWidget.Opacity(m_wWhiteFlash, 0, PAINEFFECT_PROGRESSION_FADEOUT_1_DURATION);
		AnimateWidget.AlphaMask(m_wWhiteFlash, 0, PAINEFFECT_OPACITY_FADEOUT_1_DURATION);
		
		if (repeat && m_bHasPainEffect)
			GetGame().GetCallqueue().CallLater(CreateEffectOverTime, PAIN_REPEAT_DELAY, false, !isFadeIn);
	}

	//------------------------------------------------------------------------------------------------
	//! Clear pain effect over time
	override void ClearEffects()
	{
		m_bHasPainEffect = 0;
		
		if (m_wWhiteFlash)
		{
			AnimateWidget.StopAllAnimations(m_wWhiteFlash);
			m_wWhiteFlash.SetOpacity(0);
			m_wWhiteFlash.SetMaskProgress(0);
		};
		
		GetGame().GetCallqueue().Remove(CreateEffectOverTime);
		GetGame().GetCallqueue().Remove(ClearEffectOverTime);
		
		if (!m_pDamageManager)
			return;

		m_pPainHZ.GetOnDamageStateChanged().Remove(OnPainStateChanged);
	}
}