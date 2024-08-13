//------------------------------------------------------------------------------------------------
//! ACE morphine effect, which replaces SCR_ConsumableMorphine in SCR_ConsumableItemComponent
//! of MorphineInjection_01.et
[BaseContainerProps()]
class ACE_Medical_ConsumableMorphine : SCR_ConsumableEffectHealthItems
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
	void ACE_Medical_ConsumableMorphine()
	{
		m_eConsumableType = SCR_EConsumableType.MORPHINE;
	}
}
