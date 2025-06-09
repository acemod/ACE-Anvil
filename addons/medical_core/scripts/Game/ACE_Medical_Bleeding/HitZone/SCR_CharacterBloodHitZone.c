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
		if (!EntityUtils.IsPlayer(GetOwner()))
			return true;
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (!settings)
			return true;
		
		return settings.m_bBleedOutForPlayersEnabled;
	}
}
