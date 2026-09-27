//------------------------------------------------------------------------------------------------
//! Add second chance data
modded class SCR_CharacterDamageManagerComponent : SCR_DamageManagerComponent
{
	protected float m_bACE_Medical_WasSecondChanceGranted = false;
	protected float m_fACE_Medical_SecondChanceDeactivationTimeMS = -float.INFINITY;
	protected float m_fACE_Medical_LastFallDamageTimeMS = -float.INFINITY;
	
	//-----------------------------------------------------------------------------------------------------------
	[Friend(ACE_Medical_SecondChanceSystem)]
	protected void ACE_Medical_OnSecondChanceGranted()
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
		m_fACE_Medical_SecondChanceDeactivationTimeMS = GetOwner().GetWorld().GetWorldTime() + s_ACE_Medical_Core_Settings.m_iSecondChanceDeactivationTimeoutMs;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	bool ACE_Medical_ShouldDeactivateSecondChance()
	{
		if (m_fACE_Medical_SecondChanceDeactivationTimeMS < 0)
			return false;
		
		// Check if second chance has expired
		return GetOwner().GetWorld().GetWorldTime() >= m_fACE_Medical_SecondChanceDeactivationTimeMS;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	bool ACE_Medical_IsSecondChanceEnabled()
	{
		// Check for AI - if AI setting is disabled, block second chance for AI and possessed AI
		if (!s_ACE_Medical_Core_Settings.m_bSecondChanceForAIEnabled)
		{
			SCR_ECharacterControlType controlType = SCR_CharacterHelper.GetCharacterControlType(GetOwner());
			if (controlType == SCR_ECharacterControlType.AI || controlType == SCR_ECharacterControlType.POSSESSED_AI)
				return false;
		}

		// Check for fall damage
		if (s_ACE_Medical_Core_Settings.m_bSecondChanceForFallDamageEnabled || m_fACE_Medical_SecondChanceDeactivationTimeMS < 0)
			return true;
		
		if (m_fACE_Medical_SecondChanceDeactivationTimeMS - m_fACE_Medical_LastFallDamageTimeMS <= s_ACE_Medical_Core_Settings.m_iSecondChanceDeactivationTimeoutMs)
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
		m_fACE_Medical_SecondChanceDeactivationTimeMS = -float.INFINITY;
		
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
		
		if (m_bACE_Medical_WasSecondChanceGranted)
			m_fACE_Medical_ResilienceRegenScale = Math.Min(m_fACE_Medical_ResilienceRegenScale, s_ACE_Medical_Core_Settings.m_fSecondChanceResilienceRegenScale);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void ACE_Medical_RecordFallDamageTime()
	{
		if (!Replication.IsServer())
			return;
		
		m_fACE_Medical_LastFallDamageTimeMS = GetOwner().GetWorld().GetWorldTime();
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Store the time of the latest fall damage
	override void HandleAnimatedFallDamage(float damage)
	{
		super.HandleAnimatedFallDamage(damage);
		
		ACE_Medical_RecordFallDamageTime();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Store the time of the latest fall damage
	override void HandleRagdollFallDamage(notnull HitZone contactingHitZone, float damage)
	{
		super.HandleRagdollFallDamage(contactingHitZone, damage);
		
		ACE_Medical_RecordFallDamageTime();
	}
}
