//-----------------------------------------------------------------------------------------------------------
modded class SCR_CharacterBloodHitZone : SCR_RegeneratingHitZone
{
	protected SCR_CharacterDamageManagerComponent m_pACE_Medical_DamageManager;
	
	//-----------------------------------------------------------------------------------------------------------
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		m_pACE_Medical_DamageManager = SCR_CharacterDamageManagerComponent.Cast(pManagerComponent);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Update resilience recovery scale when transitioning from and to unconscious state
	override protected void OnDamageStateChanged(EDamageState newState, EDamageState previousDamageState, bool isJIP)
	{
		super.OnDamageStateChanged(newState, previousDamageState, isJIP);
		
		if (!Replication.IsServer())
			return;
		
		if (newState == ECharacterBloodState.UNCONSCIOUS || previousDamageState == ECharacterBloodState.UNCONSCIOUS)
			m_pACE_Medical_DamageManager.ACE_Medical_UpdateResilienceRegenScale();
	}
}
