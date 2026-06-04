//------------------------------------------------------------------------------------------------
class ACE_Medical_HeatpackUserAction : SCR_HealingUserAction
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
	override bool CanBeShownScript(IEntity user)
	{		
		// Target character
		ChimeraCharacter targetCharacter = ChimeraCharacter.Cast(GetOwner());
		if (!targetCharacter)
			return false;
		
		// Medic character
		ChimeraCharacter userCharacter = ChimeraCharacter.Cast(user);
		if (!userCharacter)
			return false;
		
		// Medics' item use ability check
		CharacterControllerComponent userController = userCharacter.GetCharacterController();
		if (!userController || userController.IsUsingItem())
			return false;
		
		// Can only see healing useractions when holding respective consumable
		SCR_ConsumableItemComponent consumableComponent = GetConsumableComponent(userCharacter);
		if (!consumableComponent || consumableComponent.GetConsumableType() != m_eConsumableType)
			return false;
		
		// Cannot see healing useractions on dead people
		SCR_CharacterDamageManagerComponent damageMan = SCR_CharacterDamageManagerComponent.Cast(targetCharacter.GetDamageManager());
		if (!damageMan || damageMan.GetState() == EDamageState.DESTROYED)
			return false;
		
		if (damageMan.GetGroupIsBeingHealed(m_eHitZoneGroup))
			return false;
		
		return true;
	}
}
