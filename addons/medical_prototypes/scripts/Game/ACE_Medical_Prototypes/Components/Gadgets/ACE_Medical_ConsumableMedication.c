//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Medical_ConsumableMedication : ACE_Medical_ConsumableEffectHealthItems
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, desc: "Type of administered drug", enums: ParamEnumArray.FromEnum(ACE_Medical_EDrugType))]
	protected ACE_Medical_EDrugType m_eDrugType;
	
	[Attribute(desc: "Configuration of the dose that gets administered")]
	protected ref ACE_Medical_Dose m_Dose;

	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(notnull IEntity target, notnull IEntity user, IEntity item, ItemUseParameters animParams)
	{
		if (!Replication.IsServer())
			return;
		
		ACE_Medical_MedicationComponent medicationComponent = ACE_Medical_MedicationComponent.Cast(target.FindComponent(ACE_Medical_MedicationComponent));
		if (!medicationComponent)
			return;
		
		medicationComponent.AddMedication(m_eDrugType, m_Dose);
		
		InventoryItemComponent itemComponent = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
		if (!itemComponent)
			return;
		
		medicationComponent.AddLogEntry(itemComponent.GetUIInfo().GetName(), GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(user));
	}
}
