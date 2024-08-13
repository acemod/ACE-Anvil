//! Epinephrine effect: Heals resilience hit zone
[BaseContainerProps()]
class ACE_Medical_ConsumableEpinephrine : SCR_ConsumableEffectHealthItems
{
	//------------------------------------------------------------------------------------------------
	//! Can always be applied in advanced medical
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason)
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Can always be applied in advanced medical
	override bool CanApplyEffectToHZ(notnull IEntity target, notnull IEntity user, ECharacterHitZoneGroup group, out SCR_EConsumableFailReason failReason = SCR_EConsumableFailReason.NONE)
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Set consumable type in ctor
	void ACE_Medical_ConsumableEpinephrine()
	{
		m_eConsumableType = SCR_EConsumableType.ACE_MEDICAL_EPINEPHRINE;
	}
}
