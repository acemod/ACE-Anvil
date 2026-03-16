modded class ACE_Medical_VitalsComponent
{
	protected ACE_Medical_Defibrillation_ECardiacRhythm m_eCardiacRhythm = ACE_Medical_Defibrillation_ECardiacRhythm.Unknown;
	protected int m_iShocksDelivered = 0;
	
	//------------------------------------------------------------------------------------------------
	void SetCardiacRhythm(ACE_Medical_Defibrillation_ECardiacRhythm rhythm)
	{
		m_eCardiacRhythm = rhythm;
	}

	//------------------------------------------------------------------------------------------------
	ACE_Medical_Defibrillation_ECardiacRhythm GetCardiacRhythm()
	{
		return m_eCardiacRhythm;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetShocksDelivered(int value)
	{
		m_iShocksDelivered = value;
	}
	
	//------------------------------------------------------------------------------------------------
	void ModifyShocksDelivered(int value)
	{
		m_iShocksDelivered += value;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetShocksDelivered()
	{
		return m_iShocksDelivered;
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnRevived()
	{
		super.OnRevived();
		m_iShocksDelivered = 0;
	}
}