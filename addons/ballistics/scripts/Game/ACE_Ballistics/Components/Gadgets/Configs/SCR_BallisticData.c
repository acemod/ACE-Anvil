//------------------------------------------------------------------------------------------------
modded class SCR_BallisticData : ScriptAndConfig
{
	protected SCR_EOpticsAngleUnits m_eACE_UnitType;

	//------------------------------------------------------------------------------------------------
	void ACE_SetUnitType(SCR_EOpticsAngleUnits unitType)
	{
		m_eACE_UnitType = unitType;
	}
	
	//------------------------------------------------------------------------------------------------
	SCR_EOpticsAngleUnits ACE_GetUnitType()
	{
		return m_eACE_UnitType;
	}
}
