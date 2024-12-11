//------------------------------------------------------------------------------------------------
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	[RplProp()]
	protected bool m_bACE_Carrying_IsCarrier = false;
	[RplProp()]
	protected bool m_bACE_Carrying_IsCarried = false;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Carrying_SetIsCarrier(bool isCarrier)
	{
		m_bACE_Carrying_IsCarrier = isCarrier;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool ACE_Carrying_IsCarrier()
	{
		return m_bACE_Carrying_IsCarrier;
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Carrying_SetIsCarried(bool isCarried)
	{
		m_bACE_Carrying_IsCarried = isCarried;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool ACE_Carrying_IsCarried()
	{
		return m_bACE_Carrying_IsCarried;
	}
}
