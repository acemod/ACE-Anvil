//------------------------------------------------------------------------------------------------
class ACE_Medical_ChestSealUserAction : SCR_HealingUserAction
{
	//------------------------------------------------------------------------------------------------
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
				
			return false;
		}
		
		return true;
	}
}
