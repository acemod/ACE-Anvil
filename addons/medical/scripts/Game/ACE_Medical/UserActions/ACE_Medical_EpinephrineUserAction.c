//------------------------------------------------------------------------------------------------
class ACE_Medical_EpinephrineUserAction : SCR_MorphineUserAction
{
	[Attribute(defvalue: "#ACE_Medical-FailReason_NotUnconscious", desc: "String for when target is not incapacitated")]
	protected LocalizedString m_sNotIncapacitated;
	
	[Attribute(defvalue: "#AR-Inventory_Bleeding", desc: "String for when target is bleeding")]
	protected LocalizedString m_sIsBleeding;
	
	[Attribute(defvalue: "#ACE_Medical-FailReason_TooInjured", desc: "String for when target is too damaged")]
	protected LocalizedString m_sTooDamaged;
	
	[Attribute(defvalue: "#ACE_Medical-FailReason_CriticallyInjured", desc: "String for when target is critically injured")]
	protected LocalizedString m_sCriticallyInjured;
	
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
			if (reason == SCR_EConsumableFailReason.ACE_MEDICAL_NOT_INCAPACITATED)
				SetCannotPerformReason(m_sNotIncapacitated);		
			else if (reason == SCR_EConsumableFailReason.ALREADY_APPLIED)
				SetCannotPerformReason(m_sAlreadyApplied);
			else if (reason == SCR_EConsumableFailReason.IS_BLEEDING)
				SetCannotPerformReason(m_sIsBleeding);
			else if (reason == SCR_EConsumableFailReason.ACE_MEDICAL_TOO_DAMAGED)
				SetCannotPerformReason(m_sTooDamaged);
			else if (reason == SCR_EConsumableFailReason.ACE_MEDICAL_CRITICALLY_INJURED)
				SetCannotPerformReason(m_sCriticallyInjured);
			
			return false;
		}
		
		return true;
	}
}
