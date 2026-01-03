modded class ACE_Medical_VitalsComponent
{
	protected int m_iShocksDelivered = 0;
	
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