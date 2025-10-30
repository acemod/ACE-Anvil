//------------------------------------------------------------------------------------------------
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	protected static ACE_Medical_Core_Settings s_ACE_Medical_Core_Settings;

	protected SCR_CharacterHitZone m_ACE_Medical_LastStruckPhysicalHitZone;
	protected SCR_CharacterHealthHitZone m_ACE_Medical_HealthHitZone;
	protected float m_fACE_Medical_ResilienceRegenScale = 1;
	
	[RplProp()]
	protected float m_fACE_Medical_MinHealthScaledForEpinephrine = 0.33;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Initialize members
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
		
		m_pACE_Medical_HealthHitZone = SCR_CharacterHealthHitZone.Cast(GetHitZoneByName("Health"));
		
		if (!s_pACE_Medical_Core_Settings)
			s_pACE_Medical_Core_Settings = ACE_SettingsHelperT<ACE_Medical_Core_Settings>.GetModSettings();
		
		if (s_pACE_Medical_Core_Settings)
		{
			m_fACE_Medical_ResilienceRegenScale = s_pACE_Medical_Core_Settings.m_fDefaultResilienceRegenScale;
			m_fACE_Medical_MinHealthScaledForEpinephrine = s_pACE_Medical_Core_Settings.m_fMinHealthScaledForEpinephrine;
			Replication.BumpMe();
		}
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Update last struck physical hit zone
	override void OnDamage(notnull BaseDamageContext damageContext)
	{
		super.OnDamage(damageContext);
		
		if (!Replication.IsServer())
			return;
		
		SCR_CharacterHitZone struckPhysicalHitZone = SCR_CharacterHitZone.Cast(damageContext.struckHitZone);
		if (struckPhysicalHitZone)
			m_pACE_Medical_LastStruckPhysicalHitZone = struckPhysicalHitZone;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	protected override void OnDamageStateChanged(EDamageState state)
	{
		super.OnDamageStateChanged(state);
		
		if (Replication.IsServer() && (state == EDamageState.DESTROYED))
			ACE_Medical_OnKilled();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	protected void ACE_Medical_OnKilled();
	
	//-----------------------------------------------------------------------------------------------------------
	//! Returns last stuck physical hit zone
	SCR_CharacterHitZone ACE_Medical_GetLastStruckPhysicalHitZone()
	{
		return m_pACE_Medical_LastStruckPhysicalHitZone;
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_UpdateResilienceRegenScale()
	{
		if (m_pBloodHitZone.GetDamageState() == ECharacterBloodState.UNCONSCIOUS)
			m_fACE_Medical_ResilienceRegenScale = 0;
		else
			m_fACE_Medical_ResilienceRegenScale = s_pACE_Medical_Core_Settings.m_fDefaultResilienceRegenScale;
	}
	
	//------------------------------------------------------------------------------------------------
	float ACE_Medical_GetResilienceRegenScale()
	{
		return m_fACE_Medical_ResilienceRegenScale;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check if epinephrine can be applied to this character
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
		
		// Check if epinephrine is in the system already
		array<ref SCR_PersistentDamageEffect> effects = GetAllPersistentEffectsOfType(ACE_Medical_EpinephrineDamageEffect);
		if (!effects.IsEmpty())
		{
			failReason = SCR_EConsumableFailReason.ALREADY_APPLIED;
			return false;
		}
		
		// Cannot be applied while bleeding
		if (IsBleeding())
		{
			failReason = SCR_EConsumableFailReason.IS_BLEEDING;
			return false;
		}
		
		// Check if too injured
		if (m_pACE_Medical_HealthHitZone.GetHealthScaled() < m_fACE_Medical_MinHealthScaledForEpinephrine)
		{
			failReason = SCR_EConsumableFailReason.ACE_MEDICAL_TOO_DAMAGED;
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Returns overall scaled health of the character
	//! Should be used instead of GetHealthScaled
	float ACE_Medical_GetHealthScaled()
	{
		return m_pACE_Medical_HealthHitZone.GetHealthScaled();
	}
}
