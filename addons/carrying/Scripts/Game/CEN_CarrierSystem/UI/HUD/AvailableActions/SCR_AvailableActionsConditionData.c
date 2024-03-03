modded class SCR_AvailableActionsConditionData
{
	//! Is character carrying?
	protected bool m_CEN_CarrierSystem_bIsCarrier;

	//------------------------------------------------------------------------------------------------
	//! Clears all variables by setting them to their default state
	//! Also invalidates this data
	protected void CEN_CarrierSystem_Clear()
	{
		m_CEN_CarrierSystem_bIsCarrier = false;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns whether character is carrying
	bool CEN_CarrierSystem_GetIsCharacterCarrier()
	{
		return m_CEN_CarrierSystem_bIsCarrier;
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
		CEN_CarrierSystem_Clear();

		m_CEN_CarrierSystem_bIsCarrier = CEN_CarrierSystem_Helper.IsCarrier(controlledEntity);
	}
}
