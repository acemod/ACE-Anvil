//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class CEN_CarrierSystem_IsCharacterCarrierCondition : SCR_AvailableActionCondition
{
	//------------------------------------------------------------------------------------------------
	//! Returns true if the release action is available
	override bool IsAvailable(SCR_AvailableActionsConditionData data)
	{		
		if (!data)
			return false;
		
		return GetReturnResult(data.CEN_CarrierSystem_GetIsCharacterCarrier());
	}
}