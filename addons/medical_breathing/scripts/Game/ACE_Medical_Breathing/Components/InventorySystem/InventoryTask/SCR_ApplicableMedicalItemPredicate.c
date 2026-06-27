//------------------------------------------------------------------------------------------------
//! Add chest seal for self application
modded class SCR_ApplicableMedicalItemPredicate : InventorySearchPredicate
{
	//------------------------------------------------------------------------------------------------
	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{
		if (super.IsMatch(storage, item, queriedComponents, queriedAttributes))
			return true;
		
		SCR_EConsumableType type = SCR_ConsumableItemComponent.Cast(queriedComponents[0]).GetConsumableType();
		if (type != SCR_EConsumableType.ACE_MEDICAL_CHEST_SEAL)
			return false;

		SCR_ConsumableItemComponent medicalItem = SCR_ConsumableItemComponent.Cast(item.FindComponent(SCR_ConsumableItemComponent));
		SCR_ConsumableEffectHealthItems effect = SCR_ConsumableEffectHealthItems.Cast(medicalItem.GetConsumableEffect());
		if (!effect)
			return false;

		return effect.CanApplyEffectToHZ(characterEntity, characterEntity, hitZoneGroup);
	}
}
