//------------------------------------------------------------------------------------------------
class ACE_Trenches_SupportStationComponentClass : SCR_BaseSupportStationComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_Trenches_SupportStationComponent : SCR_BaseSupportStationComponent
{
	//------------------------------------------------------------------------------------------------
	override ESupportStationType GetSupportStationType()
	{
		return ESupportStationType.ACE_TRENCHES_PROVIDER;
	}
}
