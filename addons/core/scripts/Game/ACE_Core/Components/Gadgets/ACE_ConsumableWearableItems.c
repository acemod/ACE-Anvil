//------------------------------------------------------------------------------------------------
//! Used for handheld items 
[BaseContainerProps()]
class ACE_ConsumableWearableItems : SCR_ConsumableEffectHealthItems
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, desc: "Type of wearable", enums: ParamEnumArray.FromEnum(SCR_EConsumableType))]
	protected SCR_EConsumableType m_eWearableType;
	
	//------------------------------------------------------------------------------------------------
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason)
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanApplyEffectToHZ(notnull IEntity target, notnull IEntity user, ECharacterHitZoneGroup group, out SCR_EConsumableFailReason failReason = SCR_EConsumableFailReason.NONE)
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	
	override void ApplyEffect(notnull IEntity target, notnull IEntity user, IEntity item, ItemUseParameters animParams)
	{
		SCR_InventoryStorageManagerComponent userInventoryManager = SCR_InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!userInventoryManager)
			return;
		
		BaseLoadoutClothComponent targetLoadoutCloth = BaseLoadoutClothComponent.Cast(item.FindComponent(BaseLoadoutClothComponent));
		if (!targetLoadoutCloth)
			return;
	
		EquipedLoadoutStorageComponent targetLoadout = EquipedLoadoutStorageComponent.Cast(target.FindComponent(EquipedLoadoutStorageComponent));
		if (!targetLoadout)
			return;
	
		LoadoutSlotInfo targetSlot = targetLoadout.GetSlotFromArea(targetLoadoutCloth.GetAreaType().Type());
		if (!targetSlot)
			return;
		
		IEntity oldItem = targetSlot.GetAttachedEntity();
		if (oldItem)
		{
			DropAndMoveOperationCallback chainedCallback = new DropAndMoveOperationCallback();
			chainedCallback.m_Manager = userInventoryManager;
			chainedCallback.m_ItemAfter = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
			chainedCallback.m_ItemBefore = InventoryItemComponent.Cast(oldItem.FindComponent(InventoryItemComponent));
			chainedCallback.m_TargetSlot = targetSlot;
			chainedCallback.m_bIstakenFromArsenal = false;
			userInventoryManager.TryRemoveItemFromStorage(oldItem, targetLoadout, chainedCallback);
		}
		else
		{
			userInventoryManager.TryReplaceAndDropItemAtSlot(targetLoadout, item, targetSlot.GetID());
		}

		super.ApplyEffect(target, user, item, animParams);
	}

	//------------------------------------------------------------------------------------------------
	//! Set consumable type in ctor
	void ACE_ConsumableWearableItems()
	{
		m_eConsumableType = m_eWearableType;
	}
}
