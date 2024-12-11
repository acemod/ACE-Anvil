//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Medical_ConsumableEffectHealthItems : SCR_ConsumableEffectHealthItems
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, desc: "Type of device that provides the health effect", enums: ParamEnumArray.FromEnum(SCR_EConsumableType))]
	protected SCR_EConsumableType m_eDeviceType;
	
	[Attribute(defvalue: "NONE", desc: "Whether it gets attached to a dedicated equipement slot on the patient", uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(ACE_EEquipementSlot))]
	protected ACE_EEquipementSlot m_eSlotToAttach;
	
	//------------------------------------------------------------------------------------------------
	//! Can always be applied in advanced medical
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason)
	{
		if (m_eSlotToAttach == ACE_EEquipementSlot.NONE)
			return true;
		
		ACE_EquipmentStorageComponent storageComponent = ACE_EquipmentStorageComponent.Cast(target.FindComponent(ACE_EquipmentStorageComponent));
		if (!storageComponent)
			return false;
		
		if (storageComponent.IsSlotOccupied(m_eSlotToAttach))
		{
			failReason = SCR_EConsumableFailReason.ALREADY_APPLIED;
			return false;
		}
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Can always be applied in advanced medical
	override bool CanApplyEffectToHZ(notnull IEntity target, notnull IEntity user, ECharacterHitZoneGroup group, out SCR_EConsumableFailReason failReason = SCR_EConsumableFailReason.NONE)
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(notnull IEntity target, notnull IEntity user, IEntity item, ItemUseParameters animParams)
	{
		if (m_eSlotToAttach != ACE_EEquipementSlot.NONE)
		{
			ACE_EquipmentStorageComponent storageComponent = ACE_EquipmentStorageComponent.Cast(target.FindComponent(ACE_EquipmentStorageComponent));
			if (!storageComponent)
				return;
			
			storageComponent.AddItemToSlot(target, item, m_eSlotToAttach);
		}
		
		super.ApplyEffect(target, user, item, animParams);
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_ConsumableEffectHealthItems()
	{
		m_eConsumableType = m_eDeviceType;
	}
}
