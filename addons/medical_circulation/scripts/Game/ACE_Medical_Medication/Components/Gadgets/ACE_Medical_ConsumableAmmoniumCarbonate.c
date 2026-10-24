//! Epinephrine effect: Heals resilience hit zone
[BaseContainerProps()]
class ACE_Medical_ConsumableAmmoniumCarbonate : SCR_ConsumableEffectHealthItems
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
	//! Add chance for applying effect
	override void ApplyEffect(notnull IEntity target, notnull IEntity user, IEntity item, ItemUseParameters animParams)
	{
		SCR_ChimeraCharacter targetChar = SCR_ChimeraCharacter.Cast(target);
		if (!targetChar)
			return;
		
		// Has only effect when vitals are stable
		ACE_Medical_VitalsComponent vitalsComponent = ACE_Medical_VitalsComponent.Cast(targetChar.FindComponent(ACE_Medical_VitalsComponent));
		if (!vitalsComponent || vitalsComponent.GetVitalStateID() != ACE_Medical_EVitalStateID.STABLE)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(targetChar.GetDamageManager());
		if (!damageManager)
			return;
		
		float scale = damageManager.ACE_Medical_GetResilienceRegenScale();
		
		ACE_Medical_Circulation_Settings settings = ACE_SettingsHelperT<ACE_Medical_Circulation_Settings>.GetModSettings();;
		if (!settings)
			return;
		
		// TO DO: Add time of last attempt as a factor to prevent spamming to become meta
		if (Math.RandomFloat(0, 1) > Math.Map(scale, 0, settings.m_fMaxRevivalResilienceRecoveryScale, settings.m_fAmmoniumCarbonateSuccessChanceMin, settings.m_fAmmoniumCarbonateSuccessChanceMax))
			return;
		
		super.ApplyEffect(target, user, item, animParams);
		
		ACE_Medical_MedicationComponent medicationComponent = ACE_Medical_MedicationComponent.Cast(target.FindComponent(ACE_Medical_MedicationComponent));
		if (!medicationComponent)
			return;
		
		InventoryItemComponent itemComponent = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
		if (!itemComponent)
			return;
		
		if (Replication.IsServer())
			medicationComponent.AddLogEntry(itemComponent.GetUIInfo().GetName(), GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(user));
	}

	//------------------------------------------------------------------------------------------------
	//! Set consumable type in ctor
	void ACE_Medical_ConsumableAmmoniumCarbonate()
	{
		m_eConsumableType = SCR_EConsumableType.ACE_MEDICAL_AMMONIUM_CARBONATE;
	}
}
