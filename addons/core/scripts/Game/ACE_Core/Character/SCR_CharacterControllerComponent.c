//------------------------------------------------------------------------------------------------
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	[RplProp()]
	protected bool m_bACE_IsCarrier = false;
	[RplProp()]
	protected bool m_bACE_IsCarried = false;
	
	//------------------------------------------------------------------------------------------------
	void ACE_SetIsCarrier(bool isCarrier)
	{
		m_bACE_IsCarrier = isCarrier;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool ACE_IsCarrier()
	{
		return m_bACE_IsCarrier;
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_SetIsCarried(bool isCarried)
	{
		m_bACE_IsCarried = isCarried;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	bool ACE_IsCarried()
	{
		return m_bACE_IsCarried;
	}
}
