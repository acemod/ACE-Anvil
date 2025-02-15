//------------------------------------------------------------------------------------------------
modded class SCR_AvailableActionsConditionData
{
	//! Is character carrying?
	protected bool m_bACE_IsCarrier;

	//------------------------------------------------------------------------------------------------
	//! Clears all variables by setting them to their default state
	//! Also invalidates this data
	protected void ACE_Clear()
	{
		m_bACE_IsCarrier = false;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns whether character is carrying
	bool ACE_GetIsCharacterCarrier()
	{
		return m_bACE_IsCarrier;
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
		ACE_Clear();
		
		SCR_CharacterControllerComponent charCtrl = SCR_CharacterControllerComponent.Cast(controlledEntity.FindComponent(SCR_CharacterControllerComponent));
		if (charCtrl)
			m_bACE_IsCarrier = charCtrl.ACE_IsCarrier();
	}
}
