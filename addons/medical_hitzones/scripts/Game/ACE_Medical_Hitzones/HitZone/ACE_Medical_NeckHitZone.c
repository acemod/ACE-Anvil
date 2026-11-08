//-----------------------------------------------------------------------------------------------------------
modded class ACE_Medical_NeckHitZone : SCR_CharacterHitZone
{
	//-----------------------------------------------------------------------------------------------------------
	//! Add bleeding when wounded and knock the character out
	override void OnDamageStateChanged(EDamageState newState, EDamageState previousDamageState, bool isJIP)
	{
		super.OnDamageStateChanged(newState, previousDamageState, isJIP);
		
		if (!Replication.IsServer() || newState <= ECharacterDamageState.INTERMEDIARY)
			return;
		
		SCR_CharacterDamageManagerComponent charDamageManager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
		if (!charDamageManager)
			return;
		
		charDamageManager.AddBleedingEffectOnHitZone(this);
		
		ACE_Medical_Hitzones_Settings settings = ACE_SettingsHelperT<ACE_Medical_Hitzones_Settings>.GetModSettings();
		if (settings && settings.m_bInstantUnconOnMassiveBleeding)
			GetGame().GetCallqueue().Call(charDamageManager.GetResilienceHitZone().SetHealthScaled, 0);
	}
}
