//------------------------------------------------------------------------------------------------
class ACE_Medical_HeatpackUserAction : SCR_MorphineUserAction
{
	[Attribute(defvalue: "#ACE_Medical-FailReason_NotUnconscious", desc: "String for when target is not incapacitated")]
	protected LocalizedString m_sNotIncapacitated;
	
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
			return false;
		}
		
		return true;
	}
}
