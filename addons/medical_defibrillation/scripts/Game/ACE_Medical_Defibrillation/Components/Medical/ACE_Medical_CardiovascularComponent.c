modded class ACE_Medical_CardiovascularComponent
{
	// Needs replication
	protected ACE_Medical_ECardiacRhythm m_eCardiacRhythm = ACE_Medical_ECardiacRhythm.Sinus;
	
	protected static const ref array<ACE_Medical_ECardiacRhythm> pulselessRhythms = { ACE_Medical_ECardiacRhythm.VF, ACE_Medical_ECardiacRhythm.PEA };
	
	// Needs replication
	protected int m_iShocksDelivered = 0;
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_ECardiacRhythm GetCardiacRhythm()
	{
		return m_eCardiacRhythm;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetCardiacRhythm(ACE_Medical_ECardiacRhythm rhythm)
	{
		m_eCardiacRhythm = rhythm;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetShocksDelivered()
	{
		return m_iShocksDelivered;
	}
	
	//------------------------------------------------------------------------------------------------
	int AddShocksDelivered(int value)
	{
		m_iShocksDelivered = m_iShocksDelivered + 1;
		return m_iShocksDelivered;
	}
}