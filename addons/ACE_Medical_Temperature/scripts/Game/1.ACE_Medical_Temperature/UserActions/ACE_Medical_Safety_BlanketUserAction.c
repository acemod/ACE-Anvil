class ACE_Medical_SafetyBlanketUserAction : SCR_MorphineUserAction 
{
	override bool CanBeShownScript(IEntity user)
	{
		if (!super.CanBeShownScript(user))
		{
			return false;
		}
		if (SCR_CharacterControllerComponent.Cast(GetOwner().FindComponent(SCR_CharacterControllerComponent)).GetLifeState()!=ECharacterLifeState.INCAPACITATED)
		{
			return false;
		}
		return true;
	}
}