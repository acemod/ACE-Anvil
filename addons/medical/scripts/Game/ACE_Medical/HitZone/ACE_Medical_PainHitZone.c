//-----------------------------------------------------------------------------------------------------------
//! Pain - does not receive damage directly, only via scripted events.
class ACE_Medical_PainHitZone : SCR_RegeneratingHitZone
{
    //-----------------------------------------------------------------------------------------------------------
    //! Store instance on SCR_CharacterDamageManagerComponent on init
    override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
    {
	super.OnInit(pOwnerEntity, pManagerComponent);
	SCR_CharacterDamageManagerComponent characterDamageManager = SCR_CharacterDamageManagerComponent.Cast(pManagerComponent);
	if (characterDamageManager)
	    characterDamageManager.ACE_Medical_SetPainHitZone(this);
    }

    //-----------------------------------------------------------------------------------------------------------
    //! Suppress passive regeneration while bleeding
    override float CalculatePassiveRegeneration()
    {
	SCR_CharacterDamageManagerComponent manager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
	if (manager && manager.IsDamagedOverTime(EDamageType.BLEEDING))
	    return 0;

	return super.CalculatePassiveRegeneration();
    }
}
