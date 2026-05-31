[BaseContainerProps()]
class ACE_Temperature_ConsumableHeatpack : SCR_ConsumableEffectHealthItems
{
	//------------------------------------------------------------------------------------------------
	//! Can be applied when patient is in pain and no morphine is in the system
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason)
	{	
		ChimeraCharacter targetChar = ChimeraCharacter.Cast(target);
		if (!targetChar)
			return false;
		
		SCR_CharacterDamageManagerComponent targetDamageManager = SCR_CharacterDamageManagerComponent.Cast(targetChar.GetDamageManager());
		if (!targetDamageManager)
			return false;
		
		return true;
	}
	//------------------------------------------------------------------------------------------------
	//! Set consumable type in ctor
	void ACE_Temperature_ConsumableHeatpack()
	{
		m_eConsumableType = SCR_EConsumableType.ACE_MEDICAL_HEATPACK;
	}

	override void ApplyEffect(notnull IEntity target, notnull IEntity user, IEntity item, ItemUseParameters animParams)
	{
		if (!Replication.IsServer())
			return;
		ACE_Medical_MedicationComponent medicationComponent = ACE_Medical_MedicationComponent.Cast(target.FindComponent(ACE_Medical_MedicationComponent));
		SCR_DamageManagerComponent damageManager = SCR_DamageManagerComponent.Cast(target.FindComponent(DamageManagerComponent));
		if (!damageManager)
			return;
		
		
		
		InventoryItemComponent itemComponent = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
		if (!itemComponent)
			return;
		if (medicationComponent){
			medicationComponent.AddLogEntry(itemComponent.GetUIInfo().GetName(), GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(user));
		}
		
	}
}