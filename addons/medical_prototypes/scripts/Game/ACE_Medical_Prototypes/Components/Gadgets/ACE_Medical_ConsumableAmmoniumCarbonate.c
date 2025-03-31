//! Epinephrine effect: Heals resilience hit zone
[BaseContainerProps()]
class ACE_Medical_ConsumableAmmoniumCarbonate : SCR_ConsumableEffectHealthItems
{
	[Attribute(defvalue: "0.2", desc: "Probability for applying effect when resilience recovery scale is close to 0")]
	protected float m_fSuccessChanceMin;
	
	[Attribute(defvalue: "1", desc: "Probability for applying effect when resilience recovery scale is at ACE_Medical_CardiovascularComponent.m_fMaxRevivalResilienceRecoveryScale or above")]
	protected float m_fSuccessChanceMax;
	
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
		ACE_Medical_CardiovascularComponent cardiovascularComponent = ACE_Medical_CardiovascularComponent.Cast(target.FindComponent(ACE_Medical_CardiovascularComponent));
		if (!cardiovascularComponent)
			return;
		
		float scale = cardiovascularComponent.GetResilienceRecoveryScale();
		if (scale <= 0)
			return;
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();;
		if (!settings || !settings.m_CardiovascularSystem)
			return;
		
		// TO DO: Add time of last attempt as a factor to prevent spamming to become meta
		if (Math.RandomFloat01() > Math.Map(scale, 0, settings.m_CardiovascularSystem.m_fMaxRevivalResilienceRecoveryScale, m_fSuccessChanceMin, m_fSuccessChanceMax))
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
