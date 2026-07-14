//------------------------------------------------------------------------------------------------
class ACE_CBRN_ProtectiveGadgetComponentClass : ACE_WearableGadgetComponentClass
{
	[Attribute(desc: "Character areas protected when worn", uiwidget: UIWidgets.Flags, enums: ParamEnumArray.FromEnum(ACE_CBRN_ECharacterProtectedArea))]
	protected ACE_CBRN_ECharacterProtectedArea m_eProtectedAreas;
	
	//------------------------------------------------------------------------------------------------
	ACE_CBRN_ECharacterProtectedArea GetProtectedAreas()
	{
		return m_eProtectedAreas;
	}
}

//------------------------------------------------------------------------------------------------
//! Gadgets that are protective equipment, which characters can equip themselves or put on patients
class ACE_CBRN_ProtectiveGadgetComponent : ACE_WearableGadgetComponent
{
	//------------------------------------------------------------------------------------------------
	//! Signal change in protection state
	override void OnParentSlotChanged(InventoryStorageSlot oldSlot, InventoryStorageSlot newSlot)
	{
		super.OnParentSlotChanged(oldSlot, newSlot);
		
		LoadoutSlotInfo oldLoadoutSlot = LoadoutSlotInfo.Cast(oldSlot);
		LoadoutSlotInfo newLoadoutSlot = LoadoutSlotInfo.Cast(newSlot);
		if (!oldLoadoutSlot && !newLoadoutSlot)
			return;
		
		IEntity char;
		if (newLoadoutSlot)
			char = newLoadoutSlot.GetStorage().GetOwner();
		else
			char = oldLoadoutSlot.GetStorage().GetOwner();
		
		ACE_CBRN_CharacterProtectionComponent charProtection = ACE_CBRN_CharacterProtectionComponent.Cast(char.FindComponent(ACE_CBRN_CharacterProtectionComponent));
		if (!charProtection)
			return;
		
		// Delay call to ensure EquipedLoadoutStorageComponent::GetAll is up to date
		GetGame().GetCallqueue().Call(charProtection.OnProtectiveEquipmentChanged);		
	}
}
