//------------------------------------------------------------------------------------------------
//! Add second chance data
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	protected float m_bACE_Medical_WasSecondChanceGranted = false;
	protected float m_fACE_Medical_SecondChanceDeactivationTimeMS = -1;
	
	protected static const float ACE_MEDICAL_SECOND_CHANCE_DEACTIVATION_TIMEOUT_MS = 1000;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Friend method for SCR_CharacterHitZone and SCR_CharacterHealthHitZone
	void ACE_Medical_OnSecondChanceGranted()
	{
		// Only handle first second chance
		if (m_bACE_Medical_WasSecondChanceGranted)
			return;
		
		ACE_Medical_SetWasSecondChanceGranted(true);
		m_pResilienceHitZone.SetHealthScaled(0);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Second chance will be deactivated after ACE_MEDICAL_SECOND_CHANCE_DEACTIVATION_TIMEOUT_MS
	void ACE_Medical_ScheduleSecondChanceDeactivation()
	{
		m_fACE_Medical_SecondChanceDeactivationTimeMS = GetGame().GetWorld().GetWorldTime() + ACE_MEDICAL_SECOND_CHANCE_DEACTIVATION_TIMEOUT_MS;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	bool ACE_Medical_ShouldDeactivateSecondChance()
	{
		if (m_fACE_Medical_SecondChanceDeactivationTimeMS < 0)
			return false;
		
		// Check if second chance has expired
		return GetGame().GetWorld().GetWorldTime() >= m_fACE_Medical_SecondChanceDeactivationTimeMS;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	bool ACE_Medical_IsSecondChanceEnabled()
	{
		if (!m_pACE_Medical_Settings.m_bSecondChanceForAIEnabled && !EntityUtils.IsPlayer(GetOwner()))
			return false;
		
		return true;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	protected void ACE_Medical_SetWasSecondChanceGranted(bool wasGranted)
	{
		m_bACE_Medical_WasSecondChanceGranted = wasGranted;
		ACE_Medical_UpdateResilienceRegenScale();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Whether second chance was granted at least one time
	//! Gets reset when character wakes up
	bool ACE_Medical_WasSecondChanceGranted()
	{
		return m_bACE_Medical_WasSecondChanceGranted;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Unsets previously granted second chance
	void ACE_Medical_ClearSecondChanceHistory()
	{
		ACE_Medical_SetWasSecondChanceGranted(false);
		ACE_Medical_UpdateResilienceRegenScale();
		m_fACE_Medical_SecondChanceDeactivationTimeMS = -1;
		
		array<HitZone> hitZones = {};
		GetPhysicalHitZones(hitZones);
		foreach (HitZone hitZone : hitZones)
		{
			SCR_CharacterHitZone physicalHitZone = SCR_CharacterHitZone.Cast(hitZone);
			if (physicalHitZone)
				physicalHitZone.ACE_Medical_ClearSecondChanceHistory();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Override recovery scale when second chance was granted and its scale is smaller than the current
	override void ACE_Medical_UpdateResilienceRegenScale()
	{
		super.ACE_Medical_UpdateResilienceRegenScale();
		
		if (m_bACE_Medical_WasSecondChanceGranted && m_fACE_Medical_ResilienceRegenScale > m_pACE_Medical_Settings.m_fSecondChanceResilienceRegenScale)
			m_fACE_Medical_ResilienceRegenScale = m_pACE_Medical_Settings.m_fSecondChanceResilienceRegenScale;
	}
}
