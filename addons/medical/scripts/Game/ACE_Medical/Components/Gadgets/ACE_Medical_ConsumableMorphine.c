//------------------------------------------------------------------------------------------------
//! ACE morphine effect, which replaces SCR_ConsumableMorphine in SCR_ConsumableItemComponent
//! of MorphineInjection_01.et
[BaseContainerProps()]
class ACE_Medical_ConsumableMorphine : SCR_ConsumableEffectHealthItems
{
    [Attribute(defvalue: "10", desc: "Regeneration speed of related hitzone when consuming this item", category: "Regeneration")]
    protected float m_fItemRegenerationSpeedDPS;

    [Attribute(defvalue: "10", desc: "Regeneration duration of related hitzone when consuming this item in seconds", category: "Regeneration")]
    protected float m_fItemRegenerationDurationS;

    //------------------------------------------------------------------------------------------------
    //! Fully heal pain hit zone
    override void ApplyEffect(notnull IEntity target, notnull IEntity user, IEntity item, ItemUseParameters animParams)
    {
	super.ApplyEffect(target, user, item, animParams);

	ChimeraCharacter char = ChimeraCharacter.Cast(target);
	if (!char)
	    return;

	SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(char.GetDamageManager());
	if (!damageManager)
	    return;

	ACE_Medical_PainHitZone painHZ = damageManager.ACE_Medical_GetPainHitZone();
	if (!painHZ)
	    return;

	painHZ.CustomRegeneration(target, m_fItemRegenerationDurationS, m_fItemRegenerationSpeedDPS);
    }

    //------------------------------------------------------------------------------------------------
    //! Can be applied when patient is in pain and no morphine is in the system
    override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason)
    {
	ChimeraCharacter char = ChimeraCharacter.Cast(target);
	if (!char)
	    return false;

	SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(char.GetDamageManager());
	if (!damageManager)
	    return false;

	ACE_Medical_PainHitZone painHZ = damageManager.ACE_Medical_GetPainHitZone();
	if (!painHZ)
	    return false;

	// Check if morphine is in the system already
	if (painHZ.GetDamageOverTime(EDamageType.HEALING) < 0)
	{
	    failReason = SCR_EConsumableFailReason.ALREADY_APPLIED;
	    return false;
	};

	if (damageManager.ACE_Medical_IsInPain())
	    return true;

	failReason = SCR_EConsumableFailReason.UNDAMAGED;
	return false;
    }

    //------------------------------------------------------------------------------------------------
    //! Can be applied to hit zones under same conditions as CanApplyEffect
    override bool CanApplyEffectToHZ(notnull IEntity target, notnull IEntity user, ECharacterHitZoneGroup group,
				     out SCR_EConsumableFailReason failReason = SCR_EConsumableFailReason.NONE)
    {
	return CanApplyEffect(target, user, failReason);
    }

    //------------------------------------------------------------------------------------------------
    //! Set consumable type in ctor
    void ACE_Medical_ConsumableMorphine()
    {
	m_eConsumableType = SCR_EConsumableType.MORPHINE;
    }
}
