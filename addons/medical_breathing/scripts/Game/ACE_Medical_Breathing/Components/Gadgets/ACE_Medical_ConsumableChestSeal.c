//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Medical_ConsumableChestSeal : SCR_ConsumableEffectHealthItems
{
	//------------------------------------------------------------------------------------------------
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason)
	{
		SCR_ChimeraCharacter targetChar = SCR_ChimeraCharacter.Cast(target);
		if (!targetChar)
			return false;
		
		ACE_Medical_VitalsComponent vitalsComponent = ACE_Medical_VitalsComponent.Cast(targetChar.FindComponent(ACE_Medical_VitalsComponent));
		if (!vitalsComponent || vitalsComponent.GetPneumothoraxScale() <= 0)
		{
			failReason = SCR_EConsumableFailReason.UNDAMAGED;
			return false;
		}
		
		// user is item if applied to oneself
		// cannot be applied to oneself in prone stance
		if (!SCR_ChimeraCharacter.Cast(user) && targetChar.GetCharacterController().GetStance() == ECharacterStance.PRONE)
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanApplyEffectToHZ(notnull IEntity target, notnull IEntity user, ECharacterHitZoneGroup group, out SCR_EConsumableFailReason failReason = SCR_EConsumableFailReason.NONE)
	{
		if (group != ECharacterHitZoneGroup.UPPERTORSO)
			return false;
		
		return super.CanApplyEffect(target, user, failReason);
	}

	//------------------------------------------------------------------------------------------------
	//! Set consumable type in ctor
	void ACE_Medical_ConsumableChestSeal()
	{
		m_eConsumableType = SCR_EConsumableType.ACE_MEDICAL_CHEST_SEAL;
	}
}
