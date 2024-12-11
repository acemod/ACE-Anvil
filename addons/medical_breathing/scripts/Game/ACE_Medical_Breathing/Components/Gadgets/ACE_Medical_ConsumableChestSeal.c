//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Medical_ConsumableChestSeal : SCR_ConsumableEffectHealthItems
{
	//------------------------------------------------------------------------------------------------
	//! Can always be applied in advanced medical
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason)
	{
		ACE_Medical_RespiratoryComponent respiratoryComponent = ACE_Medical_RespiratoryComponent.Cast(target.FindComponent(ACE_Medical_RespiratoryComponent));
		if (!respiratoryComponent || float.AlmostEqual(respiratoryComponent.GetPneumothoraxVolume(), 0))
		{
			failReason = SCR_EConsumableFailReason.UNDAMAGED;
			return false;
		}
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Set consumable type in ctor
	void ACE_Medical_ConsumableChestSeal()
	{
		m_eConsumableType = SCR_EConsumableType.ACE_MEDICAL_CHEST_SEAL;
	}
}
