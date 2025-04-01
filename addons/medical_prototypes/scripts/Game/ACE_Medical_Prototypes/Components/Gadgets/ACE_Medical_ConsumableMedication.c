//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Medical_ConsumableMedication : ACE_Medical_ConsumableEffectHealthItems
{
	[Attribute(desc: "Configuration of the doses that gets administered")]
	protected ref array<ref ACE_Medical_Dose> m_Doses;
	
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
	override void ApplyEffect(notnull IEntity target, notnull IEntity user, IEntity item, ItemUseParameters animParams)
	{
		if (!Replication.IsServer())
			return;
		
		SCR_ChimeraCharacter targetChar = SCR_ChimeraCharacter.Cast(target);
		if (!targetChar)
			return;
		
		ACE_Medical_MedicationComponent medicationComponent = targetChar.ACE_Medical_GetMedicationComponent();
		if (!medicationComponent)
			return;
		
		foreach (ACE_Medical_Dose dose : m_Doses)
		{
			medicationComponent.AddMedication(dose);
		}
		
		InventoryItemComponent itemComponent = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
		if (!itemComponent)
			return;
		
		medicationComponent.AddLogEntry(itemComponent.GetUIInfo().GetName(), GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(user));
	}
}
