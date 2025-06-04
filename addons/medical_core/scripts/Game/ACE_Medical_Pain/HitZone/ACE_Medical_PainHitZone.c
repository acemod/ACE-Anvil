//-----------------------------------------------------------------------------------------------------------
//! Pain - does not receive damage directly, only via scripted events.
class ACE_Medical_PainHitZone : SCR_RegeneratingHitZone
{
	protected SCR_CharacterDamageManagerComponent m_pACE_Medical_DamageManager;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Store instance on SCR_CharacterDamageManagerComponent on init
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		m_pACE_Medical_DamageManager = SCR_CharacterDamageManagerComponent.Cast(pManagerComponent);
		if (m_pACE_Medical_DamageManager)
			m_pACE_Medical_DamageManager.ACE_Medical_SetPainHitZone(this);
	}
}
