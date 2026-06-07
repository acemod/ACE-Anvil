//------------------------------------------------------------------------------------------------
//! Returns true if character has a camo face
[BaseContainerProps()]
class ACE_CharacterAppliedFacepaintCondition : SCR_AvailableActionCondition
{
	//------------------------------------------------------------------------------------------------
	override bool IsAvailable(SCR_AvailableActionsConditionData data)
	{
		if (!data)
			return false;
		
		ChimeraCharacter char = data.GetCharacter();
		if (!char)
			return false;
		
		SCR_CharacterIdentityComponent identityComponent = SCR_CharacterIdentityComponent.Cast(char.FindComponent(SCR_CharacterIdentityComponent));
		if (!identityComponent)
			return false;
		
		VisualIdentity visualIdentity = identityComponent.GetIdentity().GetVisualIdentity();
		
		bool result = visualIdentity.GetHead().Contains("_camo");
		return GetReturnResult(result);
	}
}
