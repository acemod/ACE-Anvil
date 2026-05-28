//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_IsCharacterCarrierCondition : SCR_AvailableActionCondition
{
	//------------------------------------------------------------------------------------------------
	//! Returns true if the release action is available
	override bool IsAvailable(notnull SCR_AvailableActionsConditionData data)
	{
		return GetReturnResult(data.ACE_GetIsCharacterCarrier());
	}
}
