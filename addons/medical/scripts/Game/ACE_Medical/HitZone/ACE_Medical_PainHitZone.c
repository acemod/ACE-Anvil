//-----------------------------------------------------------------------------------------------------------
//! Pain - does not receive damage directly, only via scripted events.
class ACE_Medical_PainHitZone : SCR_RegeneratingHitZone
{
	//-----------------------------------------------------------------------------------------------------------
	//! Store instance on SCR_CharacterDamageManagerComponent on init
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		const SCR_CharacterDamageManagerComponent characterDamageManager = SCR_CharacterDamageManagerComponent.Cast(pManagerComponent);
		if (characterDamageManager)
			characterDamageManager.ACE_Medical_SetPainHitZone(this);
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Suppress passive regeneration while bleeding
	override float CalculatePassiveRegenDPS(bool considerRegenScale = true)
	{
		const SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
		if (damageManager && damageManager.IsBleeding())
			return 0;

		return super.CalculatePassiveRegenDPS(considerRegenScale);
	}
}
