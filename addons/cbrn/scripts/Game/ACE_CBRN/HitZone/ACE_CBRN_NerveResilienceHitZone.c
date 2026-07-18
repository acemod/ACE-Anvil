//------------------------------------------------------------------------------------------------
class ACE_CBRN_NervousSystemHitZone : SCR_RegeneratingHitZone
{
	//-----------------------------------------------------------------------------------------------------------
	//! Store instance on SCR_CharacterDamageManagerComponent on init
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(pManagerComponent);
		if (damageManager)
			damageManager.ACE_Medical_SetNervousSystemHitZone(this);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	override void OnDamageStateChanged(EDamageState newState, EDamageState previousDamageState, bool isJIP)
	{
		super.OnDamageStateChanged(newState, previousDamageState, isJIP);
		
		if (!Replication.IsServer())
			return;

		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
		if (!damageManager)
			return;
		
		if (newState == ECharacterResilienceState.UNCONSCIOUS)
			damageManager.ACE_CBRN_OnFullParalysis();
		
		damageManager.ACE_Medical_UpdateResilienceRegenScale();
		damageManager.ACE_CBRN_UpdateSpasms();
	}
}
