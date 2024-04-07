modded class SCR_AvailableActionsConditionData
{
	//! Is character carrying?
	protected bool m_ACE_Carrying_bIsCarrier;

	//------------------------------------------------------------------------------------------------
	//! Clears all variables by setting them to their default state
	//! Also invalidates this data
	protected void ACE_Carrying_Clear()
	{
		m_ACE_Carrying_bIsCarrier = false;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns whether character is carrying
	bool ACE_Carrying_GetIsCharacterCarrier()
	{
		return m_ACE_Carrying_bIsCarrier;
	}
	
	//------------------------------------------------------------------------------------------------
	// Fetch data
	//------------------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------------------
	//! Fetches data from the provided entity
	//! Sets the validity of the data which can be received via IsValid()
	override void FetchData(IEntity controlledEntity, float timeSlice)
	{
		super.FetchData(controlledEntity, timeSlice);
		
		// Invalidates and clears any data prior to following collection
		ACE_Carrying_Clear();

		m_ACE_Carrying_bIsCarrier = ACE_CarryingTools.IsCarrier(controlledEntity);
	}
}
