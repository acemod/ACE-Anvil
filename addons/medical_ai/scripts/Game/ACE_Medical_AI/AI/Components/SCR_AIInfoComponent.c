//------------------------------------------------------------------------------------------------
modded class SCR_AIInfoComponent : SCR_AIInfoBaseComponent
{
	//------------------------------------------------------------------------------------------------
	bool ACE_Medical_HasConsumable(SCR_EConsumableType type)
	{
		return m_inventoryManagerComponent.ACE_Medical_GetConsumableCount(type) > 0;
	}
}
