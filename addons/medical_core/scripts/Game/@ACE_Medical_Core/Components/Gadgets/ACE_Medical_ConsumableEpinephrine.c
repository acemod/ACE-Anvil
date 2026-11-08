//------------------------------------------------------------------------------------------------
//! Epinephrine effect: Heals resilience hit zone
[BaseContainerProps()]
class ACE_Medical_ConsumableEpinephrine : SCR_ConsumableEffectHealthItems
{
	//------------------------------------------------------------------------------------------------
	//! Can be applied when patient is unconscious and no epinephrine is in the system
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason)
	{
		SCR_ChimeraCharacter targetChar = SCR_ChimeraCharacter.Cast(target);
		if (!targetChar)
			return false;

		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(targetChar.GetDamageManager());
		if (!damageManager)
			return false;
		
		return damageManager.ACE_Medical_CanApplyEpinephrine(failReason);
	}

	//------------------------------------------------------------------------------------------------
	//! Can be applied to hit zones under same conditions as CanApplyEffect
	override bool CanApplyEffectToHZ(notnull IEntity target, notnull IEntity user, ECharacterHitZoneGroup group, out SCR_EConsumableFailReason failReason = SCR_EConsumableFailReason.NONE)
	{
		return CanApplyEffect(target, user, failReason);
	}

	//------------------------------------------------------------------------------------------------
	//! Set consumable type in ctor
	void ACE_Medical_ConsumableEpinephrine()
	{
		m_eConsumableType = SCR_EConsumableType.ACE_MEDICAL_EPINEPHRINE;
	}
}
