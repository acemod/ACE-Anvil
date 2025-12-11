//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Medical_ConsumableChestSeal : SCR_ConsumableEffectHealthItems
{
	//------------------------------------------------------------------------------------------------
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason)
	{
		ACE_Medical_VitalsComponent vitalsComponent = ACE_Medical_VitalsComponent.Cast(target.FindComponent(ACE_Medical_VitalsComponent));
		if (!vitalsComponent || vitalsComponent.GetPneumothoraxScale() <= 0)
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
