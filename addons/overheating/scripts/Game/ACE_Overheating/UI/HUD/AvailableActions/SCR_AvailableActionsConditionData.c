//------------------------------------------------------------------------------------------------
modded class SCR_AvailableActionsConditionData
{
	protected bool m_bACE_Overheating_IsCharacterWeaponJammed;

	//------------------------------------------------------------------------------------------------
	//! Clears all variables by setting them to their default state
	//! Also invalidates this data
	override protected void ACE_Clear()
	{
		super.ACE_Clear();
		m_bACE_Overheating_IsCharacterWeaponJammed = false;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns whether character is carrying
	bool ACE_Overheating_GetIsCharacterWeaponJammed()
	{
		return m_bACE_Overheating_IsCharacterWeaponJammed;
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
		
		ACE_Overheating_JammingSystem system = ACE_Overheating_JammingSystem.GetInstance(GetGame().GetWorld());
		if (!system)
			return;
		
		ACE_Overheating_BarrelComponent barrel = system.GetLocalActiveBarrel();
		if (barrel)
			m_bACE_Overheating_IsCharacterWeaponJammed = barrel.IsJammed();
	}
}
