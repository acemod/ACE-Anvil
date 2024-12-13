//------------------------------------------------------------------------------------------------
//! ACE morphine effect, which replaces SCR_ConsumableMorphine in SCR_ConsumableItemComponent
//! of MorphineInjection_01.et
[BaseContainerProps()]
class ACE_Medical_ConsumableMorphine : SCR_ConsumableEffectHealthItems
{
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
		array<ref SCR_PersistentDamageEffect> effects = damageManager.GetAllPersistentEffectsOfType(ACE_Medical_MorphineDamageEffect);
		if (!effects.IsEmpty())
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
	override bool CanApplyEffectToHZ(notnull IEntity target, notnull IEntity user, ECharacterHitZoneGroup group, out SCR_EConsumableFailReason failReason = SCR_EConsumableFailReason.NONE)
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
