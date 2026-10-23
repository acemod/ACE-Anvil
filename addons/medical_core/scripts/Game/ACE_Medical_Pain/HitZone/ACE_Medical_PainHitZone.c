//-----------------------------------------------------------------------------------------------------------
//! Pain - does not receive damage directly, only via scripted events.
class ACE_Medical_PainHitZone : SCR_RegeneratingHitZone
{
	//-----------------------------------------------------------------------------------------------------------
	//! Store instance on SCR_CharacterDamageManagerComponent on init
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(pManagerComponent);
		if (damageManager)
			damageManager.ACE_Medical_SetPainHitZone(this);
	}
}
