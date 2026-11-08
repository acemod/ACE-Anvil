//------------------------------------------------------------------------------------------------
modded class SCR_PlayerController : PlayerController
{
	protected bool m_bACE_IsPeriscopeZoomed;
	
	//------------------------------------------------------------------------------------------------
	void ACE_SetIsPeriscopeZoomed(bool value)
	{
		m_bACE_IsPeriscopeZoomed = value;
	}

	//------------------------------------------------------------------------------------------------
	bool ACE_GetIsPeriscopeZoomed()
	{
		return m_bACE_IsPeriscopeZoomed;
	}
}
