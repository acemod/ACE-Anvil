//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_CBRN_ConsumableAtropineOxime : SCR_ConsumableEffectHealthItems
{
	//------------------------------------------------------------------------------------------------
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason)
	{
		ChimeraCharacter targetChar = ChimeraCharacter.Cast(target);
		if (!targetChar)
			return false;
		
		SCR_CharacterDamageManagerComponent targetDamageManager = SCR_CharacterDamageManagerComponent.Cast(targetChar.GetDamageManager());
		if (!targetDamageManager)
			return false;
		
		if (targetDamageManager.FindDamageEffectOfType(ACE_CBRN_AtropineOximeDamageEffect))
		{
			failReason = SCR_EConsumableFailReason.ALREADY_APPLIED;
			return false;
		}
		
		ACE_CBRN_NervousSystemHitZone nervousSystem = targetDamageManager.ACE_Medical_GetNervousSystemHitZone();
		if (!nervousSystem)
			return false;
		
		if (nervousSystem.GetDamageState() == ECharacterResilienceState.UNDAMAGED)
		{
			failReason = SCR_EConsumableFailReason.UNDAMAGED;
			return false;
		}
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Can be applied to hit zones under same conditions as CanApplyEffect
	override bool CanApplyEffectToHZ(notnull IEntity target, notnull IEntity user, ECharacterHitZoneGroup group, out SCR_EConsumableFailReason failReason = SCR_EConsumableFailReason.NONE)
	{
		return CanApplyEffect(target, user, failReason);
	}

	//------------------------------------------------------------------------------------------------
	//! Set consumable type in ctor
	void ACE_CBRN_ConsumableAtropineOxime()
	{
		m_eConsumableType = SCR_EConsumableType.ACE_CBRN_ATROPINE_OXIME;
	}
}
