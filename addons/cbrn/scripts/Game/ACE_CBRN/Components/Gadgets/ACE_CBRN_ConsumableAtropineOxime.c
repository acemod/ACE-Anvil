//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_CBRN_ConsumableAtropineOxime : SCR_ConsumableEffectHealthItems
{
	//------------------------------------------------------------------------------------------------
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason)
	{
		Debug.Error("Not implemented");
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
	void ACE_CBRN_ConsumableAtropineOxime()
	{
		m_eConsumableType = SCR_EConsumableType.ACE_CBRN_ATROPINE_OXIME;
	}
}
