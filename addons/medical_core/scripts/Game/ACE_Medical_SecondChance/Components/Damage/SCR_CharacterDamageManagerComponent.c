//------------------------------------------------------------------------------------------------
//! Second Chance
//! -------------
//! Second chance is evaluated when a killing blow is dealt to the character.
//! If second chance is granted, the character survives, but gets knocked out.
//! After the first second chance got granted, it is possible to regrant second chance up to
//! ACE_MEDICAL_SECOND_CHANCE_DEACTIVATION_TIMEOUT_MS, after which no more second chances are granted
//! until the character wakes up.
//! Regranting is evaluated per hit zone. A hit zone that once granted, will always regrant.
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	protected float m_bACE_Medical_WasSecondChanceGranted = false;
	protected float m_fACE_Medical_FirstSecondChanceTimeMS = -1;
	protected SCR_CharacterHitZone m_pACE_Medical_LastStruckPhysicalHitZone;
	protected ACE_Medical_Settings m_pACE_Medical_Settings;
	
	protected static const float ACE_MEDICAL_SECOND_CHANCE_DEACTIVATION_TIMEOUT_MS = 1000;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Initialize members
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_pACE_Medical_Settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Friend method for SCR_CharacterHitZone and SCR_CharacterHealthHitZone
	void ACE_Medical_OnSecondChanceGranted()
	{
		// Only handle first second chance
		if (m_bACE_Medical_WasSecondChanceGranted)
			return;
		
		m_bACE_Medical_WasSecondChanceGranted = true;
		m_fACE_Medical_FirstSecondChanceTimeMS = GetGame().GetWorld().GetWorldTime();
		m_pResilienceHitZone.SetHealthScaled(0);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Unsets previously granted second chance
	void ACE_Medical_ClearSecondChanceHistory()
	{
		m_bACE_Medical_WasSecondChanceGranted = false;
		m_fACE_Medical_FirstSecondChanceTimeMS = -1;
		
		array<HitZone> hitZones = {};
		GetPhysicalHitZones(hitZones);
		foreach (HitZone hitZone : hitZones)
		{
			SCR_CharacterHitZone physicalHitZone = SCR_CharacterHitZone.Cast(hitZone);
			if (physicalHitZone)
				physicalHitZone.ACE_Medical_SetWasSecondChanceGranted(false);
		}
	}
	
	//-----------------------------------------------------------------------------------------------------------
	bool ACE_Medical_IsSecondChanceEnabled()
	{
		if (!m_pACE_Medical_Settings.m_bSecondChanceForAIEnabled && !EntityUtils.IsPlayer(GetOwner()))
			return false;
		
		if (!m_bACE_Medical_WasSecondChanceGranted)
			return true;
		
		// Check if second chance has expired
		return GetGame().GetWorld().GetWorldTime() - m_fACE_Medical_FirstSecondChanceTimeMS < ACE_MEDICAL_SECOND_CHANCE_DEACTIVATION_TIMEOUT_MS;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Whether second chance was granted at least one time
	//! Gets reset when character wakes up
	bool ACE_Medical_WasSecondChanceGranted()
	{
		return m_bACE_Medical_WasSecondChanceGranted;
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
	//! Returns last stuck physical hit zone
	SCR_CharacterHitZone ACE_Medical_GetLastStruckPhysicalHitZone()
	{
		return m_pACE_Medical_LastStruckPhysicalHitZone;
	}
}
