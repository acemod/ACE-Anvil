//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	protected static ACE_Medical_Core_Settings s_ACE_Medical_Core_Settings;

	protected SCR_CharacterHitZone m_ACE_Medical_LastStruckPhysicalHitZone;
	protected SCR_CharacterHealthHitZone m_ACE_Medical_HealthHitZone;
	protected float m_fACE_Medical_ResilienceRegenScale = 1;
	protected float m_fACE_Medical_ResilienceDamageScale = 1;

	protected float m_fACE_Medical_LastAdrenalineHitResponseTime = 0.0;

	protected const float ACE_MEDICAL_ADRENALINE_HIT_RESPONSE_COOLDOWN = 12.0;
	protected const float ACE_MEDICAL_ADRENALINE_HIT_RESPONSE_MIN_DAMAGE = 0.05;
	protected const float ACE_MEDICAL_ADRENALINE_HIT_RESPONSE_DURATION = 8.0;
	protected const float ACE_MEDICAL_ADRENALINE_HIT_RESPONSE_DPS = -0.04;

	[RplProp()]
	protected float m_fACE_Medical_MinHealthScaledForEpinephrine = 0.33;
	
	//-----------------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
		
		m_ACE_Medical_HealthHitZone = SCR_CharacterHealthHitZone.Cast(GetHitZoneByName("Health"));
		
		if (!s_ACE_Medical_Core_Settings)
			s_ACE_Medical_Core_Settings = ACE_SettingsHelperT<ACE_Medical_Core_Settings>.GetModSettings();
		
		if (s_ACE_Medical_Core_Settings)
		{
			m_fACE_Medical_ResilienceRegenScale = s_ACE_Medical_Core_Settings.m_fDefaultResilienceRegenScale;
			m_fACE_Medical_ResilienceDamageScale = s_ACE_Medical_Core_Settings.m_fResilienceDamageScale;
			m_fACE_Medical_MinHealthScaledForEpinephrine = s_ACE_Medical_Core_Settings.m_fMinHealthScaledForEpinephrine;
			Replication.BumpMe();
		}
	}
	
	//-----------------------------------------------------------------------------------------------------------
	override void OnDamage(notnull BaseDamageContext damageContext)
	{
		super.OnDamage(damageContext);
		
		if (!Replication.IsServer())
			return;
		
		SCR_CharacterHitZone struckPhysicalHitZone = SCR_CharacterHitZone.Cast(damageContext.struckHitZone);
		if (struckPhysicalHitZone)
			m_ACE_Medical_LastStruckPhysicalHitZone = struckPhysicalHitZone;

		ACE_Medical_ApplyAdrenalineHitResponse(damageContext);
	}

	//-----------------------------------------------------------------------------------------------------------
	protected void ACE_Medical_ApplyAdrenalineHitResponse(notnull BaseDamageContext damageContext)
	{
		if (!Replication.IsServer())
			return;

		if (damageContext.damageValue < ACE_MEDICAL_ADRENALINE_HIT_RESPONSE_MIN_DAMAGE)
			return;

		float currentTime = GetGame().GetWorld().GetWorldTime();

		if (currentTime < m_fACE_Medical_LastAdrenalineHitResponseTime + ACE_MEDICAL_ADRENALINE_HIT_RESPONSE_COOLDOWN)
			return;

		array<ref SCR_PersistentDamageEffect> effects = GetAllPersistentEffectsOfType(ACE_Medical_EpinephrineDamageEffect);
		if (!effects.IsEmpty())
			return;

		ACE_Medical_EpinephrineDamageEffect epiEffect = new ACE_Medical_EpinephrineDamageEffect();

		epiEffect.SetMaxDuration(ACE_MEDICAL_ADRENALINE_HIT_RESPONSE_DURATION);
		epiEffect.SetDPS(ACE_MEDICAL_ADRENALINE_HIT_RESPONSE_DPS);

		AddDamageEffect(epiEffect);

		m_fACE_Medical_LastAdrenalineHitResponseTime = currentTime;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	protected override void OnDamageStateChanged(EDamageState newState, EDamageState previousDamageState, bool isJIP)
	{
		super.OnDamageStateChanged(newState, previousDamageState, isJIP);
		
		if (Replication.IsServer() && (newState == EDamageState.DESTROYED))
			ACE_Medical_OnKilled();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	protected void ACE_Medical_OnKilled();
	
	//-----------------------------------------------------------------------------------------------------------
	SCR_CharacterHitZone ACE_Medical_GetLastStruckPhysicalHitZone()
	{
		return m_ACE_Medical_LastStruckPhysicalHitZone;
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_UpdateResilienceRegenScale()
	{
		if (m_pBloodHitZone.GetDamageState() == ECharacterBloodState.UNCONSCIOUS)
			m_fACE_Medical_ResilienceRegenScale = 0;
		else
			m_fACE_Medical_ResilienceRegenScale = s_ACE_Medical_Core_Settings.m_fDefaultResilienceRegenScale;
	}
	
	//------------------------------------------------------------------------------------------------
	float ACE_Medical_GetResilienceRegenScale()
	{
		return m_fACE_Medical_ResilienceRegenScale;
	}

	//------------------------------------------------------------------------------------------------
	float ACE_Medical_GetResilienceDamageScale()
	{
		return m_fACE_Medical_ResilienceDamageScale;
	}

	//------------------------------------------------------------------------------------------------
	bool ACE_Medical_CanApplyEpinephrine(out SCR_EConsumableFailReason failReason)
	{
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(GetOwner());
		if (!ownerChar)
			return false;
		
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(ownerChar.GetCharacterController());
		if (charController.GetLifeState() != ECharacterLifeState.INCAPACITATED)
		{
			failReason = SCR_EConsumableFailReason.ACE_MEDICAL_NOT_INCAPACITATED;
			return false;
		}
		
		array<ref SCR_PersistentDamageEffect> effects = GetAllPersistentEffectsOfType(ACE_Medical_EpinephrineDamageEffect);
		if (!effects.IsEmpty())
		{
			failReason = SCR_EConsumableFailReason.ALREADY_APPLIED;
			return false;
		}
		
		if (IsBleeding())
		{
			failReason = SCR_EConsumableFailReason.IS_BLEEDING;
			return false;
		}
		
		if (m_ACE_Medical_HealthHitZone.GetHealthScaled() < m_fACE_Medical_MinHealthScaledForEpinephrine)
		{
			failReason = SCR_EConsumableFailReason.ACE_MEDICAL_TOO_DAMAGED;
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	float ACE_Medical_GetHealthScaled()
	{
		return m_ACE_Medical_HealthHitZone.GetHealthScaled();
	}
}