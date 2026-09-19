//-----------------------------------------------------------------------------------------------------------
modded class SCR_CharacterBloodHitZone : SCR_RegeneratingHitZone
{
	//-----------------------------------------------------------------------------------------------------------
	//! Update resilience recovery scale when transitioning from and to unconscious state
	override protected void OnDamageStateChanged(EDamageState newState, EDamageState previousDamageState, bool isJIP)
	{
		super.OnDamageStateChanged(newState, previousDamageState, isJIP);
		
		if (!Replication.IsServer())
			return;
		
		if (newState != ECharacterBloodState.UNCONSCIOUS && previousDamageState != ECharacterBloodState.UNCONSCIOUS)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
		if (!damageManager)
			return;
		
		damageManager.ACE_Medical_UpdateResilienceRegenScale();
	}
}
