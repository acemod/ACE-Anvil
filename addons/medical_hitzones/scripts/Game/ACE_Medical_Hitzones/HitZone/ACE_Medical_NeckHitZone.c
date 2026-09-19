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
		charDamageManager.GetResilienceHitZone().SetHealthScaled(0);
	}
}
