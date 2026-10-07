//------------------------------------------------------------------------------------------------
class ACE_Medical_SplintUserAction : SCR_MorphineUserAction
{
	[Attribute(defvalue: "#AR-Inventory_Bleeding", desc: "String for when target is bleeding")]
	protected LocalizedString m_sIsBleeding;
	
	//------------------------------------------------------------------------------------------------
	//! Same as in SCR_MorphineUserAction, but handle case where target is bleeding
	override bool CanBePerformedScript(IEntity user)
	{
		// Medic character
		ChimeraCharacter userCharacter = ChimeraCharacter.Cast(user);
		if (!userCharacter)
			return false;
		
		SCR_ConsumableItemComponent consumableComponent = GetConsumableComponent(userCharacter);
		if (!consumableComponent)
			return false;
		
		SCR_ConsumableEffectHealthItems effect = SCR_ConsumableEffectHealthItems.Cast(consumableComponent.GetConsumableEffect());
		if (!effect)
			return false;
		
		int reason;
		if (!effect.CanApplyEffectToHZ(GetOwner(), userCharacter, m_eHitZoneGroup, reason))
		{
			if (reason == SCR_EConsumableFailReason.UNDAMAGED)
				SetCannotPerformReason(m_sNotDamaged);
			else if (reason == SCR_EConsumableFailReason.ALREADY_APPLIED)
				SetCannotPerformReason(m_sAlreadyApplied);
			else if (reason == SCR_EConsumableFailReason.IS_BLEEDING)
				SetCannotPerformReason(m_sIsBleeding);
			
			return false;
		}
		
		return true;
	}
}
