//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Overheating_IsCharacterWeaponJammedCondition : SCR_AvailableActionCondition
{
	//------------------------------------------------------------------------------------------------
	//! Returns true if the release action is available
	override bool IsAvailable(SCR_AvailableActionsConditionData data)
	{
		if (!data)
			return false;
		
		return GetReturnResult(data.ACE_Overheating_GetIsCharacterWeaponJammed());
	}
}
