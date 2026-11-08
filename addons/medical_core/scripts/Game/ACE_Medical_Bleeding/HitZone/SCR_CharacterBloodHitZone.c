//-----------------------------------------------------------------------------------------------------------
modded class SCR_CharacterBloodHitZone : SCR_RegeneratingHitZone
{
	protected float m_fACE_Medical_TotalBleedingAmount;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Refactored bleed out handling from SCR_CharacterDamageManagerComponent::UpdateConsciousness
	override void OnDamageStateChanged(EDamageState newState, EDamageState previousDamageState, bool isJIP)
	{
		super.OnDamageStateChanged(newState, previousDamageState, isJIP);
		
		if (!Replication.IsServer() || newState != EDamageState.DESTROYED)
			return;
		
		if (!ACE_Medical_CanBleedOut())
			return;
			
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
		if (damageManager)
			damageManager.Kill(damageManager.GetInstigator());
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Whether the character can bleed out
	protected bool ACE_Medical_CanBleedOut()
	{
		if (!ACE_EntityUtils.IsPlayer(GetOwner()))
			return true;
		
		ACE_Medical_Core_Settings settings = ACE_SettingsHelperT<ACE_Medical_Core_Settings>.GetModSettings();
		if (!settings)
			return true;
		
		return settings.m_bBleedOutForPlayersEnabled;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Should be called whenever the total bleeding rate should be recalculated
	void ACE_Medical_UpdateTotalBleedingAmount()
	{
		m_fACE_Medical_TotalBleedingAmount = 0;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
		if (!damageManager)
			return;
		
		array<ref SCR_PersistentDamageEffect> effects = {};
		damageManager.FindAllDamageEffectsOfType(SCR_BleedingDamageEffect, effects);
		
		foreach (SCR_PersistentDamageEffect effect : effects)
		{
			m_fACE_Medical_TotalBleedingAmount += ACE_Medical_ComputeBleedingRateForDamageEffect(damageManager,  SCR_BleedingDamageEffect.Cast(effect));
		}
		
		ACE_Medical_Core_Settings settings = ACE_SettingsHelperT<ACE_Medical_Core_Settings>.GetModSettings();
		if (settings && settings.m_fMaxTotalBleedingRate >= 0 && m_fACE_Medical_TotalBleedingAmount > settings.m_fMaxTotalBleedingRate)
			m_fACE_Medical_TotalBleedingAmount = settings.m_fMaxTotalBleedingRate;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	protected float ACE_Medical_ComputeBleedingRateForDamageEffect(SCR_CharacterDamageManagerComponent damageManager, SCR_BleedingDamageEffect bleedingEffect)
	{
		float rate = bleedingEffect.GetDPS();
		
		SCR_CharacterHitZone affectedHitZone = SCR_CharacterHitZone.Cast(bleedingEffect.GetAffectedHitZone());
		if (affectedHitZone && damageManager.GetGroupTourniquetted(affectedHitZone.GetHitZoneGroup()))
			rate *= damageManager.GetTourniquetStrengthMultiplier();
		
		return rate;
	}
	
	//-----------------------------------------------------------------------------------------------------------
	override float GetTotalBleedingAmount()
	{
		return m_fACE_Medical_TotalBleedingAmount;
	}
}
