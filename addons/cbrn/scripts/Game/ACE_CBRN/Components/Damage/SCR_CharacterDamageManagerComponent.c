//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	protected ACE_CBRN_NervousSystemHitZone m_ACE_CBRN_NervousSystemHitZone;
	
	protected static ACE_CBRN_Settings s_ACE_CBRN_Settings;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Initialize members
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
				
		if (!s_ACE_CBRN_Settings)
			s_ACE_CBRN_Settings = ACE_SettingsHelperT<ACE_CBRN_Settings>.GetModSettings();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Called by ACE_CBRN_NervousSystemHitZone.OnInit to initialize the hit zone
	void ACE_Medical_SetNervousSystemHitZone(HitZone hitZone)
	{
		m_ACE_CBRN_NervousSystemHitZone = ACE_CBRN_NervousSystemHitZone.Cast(hitZone);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Return the nervous system hit zone
	ACE_CBRN_NervousSystemHitZone ACE_Medical_GetNervousSystemHitZone()
	{
		return m_ACE_CBRN_NervousSystemHitZone;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Similar effect to second chance
	void ACE_CBRN_OnFullParalysis()
	{
		if (ACE_CBRN_IsFullParalysisEnabled())
			ACE_Medical_OnSecondChanceGranted();
		else
			GetGame().GetCallqueue().Call(Kill, GetInstigator());
	}
	
	//-----------------------------------------------------------------------------------------------------------
	protected bool ACE_CBRN_IsFullParalysisEnabled()
	{
		// Check setting for AI and possessed AI
		if (!s_ACE_CBRN_Settings.m_bFullParalysisForAIEnabled)
		{
			SCR_ECharacterControlType controlType = SCR_CharacterHelper.GetCharacterControlType(GetOwner());
			if (controlType == SCR_ECharacterControlType.AI || controlType == SCR_ECharacterControlType.POSSESSED_AI)
				return false;
		}

		return true;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	void ACE_CBRN_UpdateSpasms()
	{
		ACE_CBRN_SpasmSystem system = ACE_CBRN_SpasmSystem.GetInstance(GetOwner().GetWorld());
		if (!system)
			return;
		
		if (ACE_CBRN_ShouldHaveSpasms())
			system.Register(ChimeraCharacter.Cast(GetOwner()));
		else
			system.Unregister(ChimeraCharacter.Cast(GetOwner()));
	}
	
	//-----------------------------------------------------------------------------------------------------------
	protected bool ACE_CBRN_ShouldHaveSpasms()
	{
		ChimeraCharacter ownerChar = ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar)
			return false;
		
		if (ownerChar.GetCharacterController().GetLifeState() != ECharacterLifeState.ALIVE)
			return false;
		
		return (m_ACE_CBRN_NervousSystemHitZone.GetDamageStateThreshold(ECharacterResilienceState.WEAKENED) >= m_ACE_CBRN_NervousSystemHitZone.GetHealthScaled());
	}
	
	//-----------------------------------------------------------------------------------------------------------
	override protected void OnDelete(IEntity owner)
	{
		super.OnDelete(owner);
		
		ACE_CBRN_SpasmSystem system = ACE_CBRN_SpasmSystem.GetInstance(GetOwner().GetWorld());
		if (system)
			system.Unregister(ChimeraCharacter.Cast(GetOwner()));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Paralyzed patients cannot recover resilience
	override void ACE_Medical_UpdateResilienceRegenScale()
	{
		super.ACE_Medical_UpdateResilienceRegenScale();
		
		if (m_ACE_CBRN_NervousSystemHitZone.GetDamageStateThreshold(ECharacterResilienceState.WEAKENED) >= m_ACE_CBRN_NervousSystemHitZone.GetHealthScaled())
			m_fACE_Medical_ResilienceRegenScale = 0.0;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Cannot be applied to paralyzed patients
	override bool ACE_Medical_CanApplyEpinephrine(out SCR_EConsumableFailReason failReason)
	{
		if (!super.ACE_Medical_CanApplyEpinephrine(failReason))
			return false;
		
		if  (m_ACE_CBRN_NervousSystemHitZone.GetDamageStateThreshold(ECharacterResilienceState.WEAKENED) >= m_ACE_CBRN_NervousSystemHitZone.GetHealthScaled())
		{
			failReason = SCR_EConsumableFailReason.ACE_CBRN_PARALYZED;
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Remove poison
	override void FullHeal(bool ignoreHealingDOT = true)
	{
		super.FullHeal(ignoreHealingDOT);
		TerminateDamageEffectsOfType(SCR_PoisonDamageEffect);
	}
}
