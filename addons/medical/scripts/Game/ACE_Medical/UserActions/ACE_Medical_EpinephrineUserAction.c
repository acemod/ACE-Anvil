//------------------------------------------------------------------------------------------------
class ACE_Medical_EpinephrineUserAction : SCR_MorphineUserAction
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
		
		int reason;
		if (!consumableComponent.GetConsumableEffect().CanApplyEffect(GetOwner(), userCharacter, reason))
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
