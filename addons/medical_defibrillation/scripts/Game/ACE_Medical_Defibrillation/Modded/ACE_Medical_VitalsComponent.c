modded class ACE_Medical_VitalsComponent
{
	protected ACE_Medical_Defibrillation_ECardiacRhythm m_eCardiacRhythm = ACE_Medical_Defibrillation_ECardiacRhythm.Unknown;
	protected int m_iShocksDelivered = 0;
	protected float m_fTimeOfLastShock = 0;
	protected float m_iTimesArrested = 0;
	
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
	float GetTimeSinceLastShock()
	{
		float currentTime = GetGame().GetWorld().GetWorldTime();
		return currentTime - m_fTimeOfLastShock;
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetTimeSinceLastShock()
	{
		float currentTime = GetGame().GetWorld().GetWorldTime();
		m_fTimeOfLastShock = currentTime;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetTimesArrested()
	{
		return m_iTimesArrested;
	}
	
	//------------------------------------------------------------------------------------------------
	void ModifyTimesArrested(int value)
	{
		m_iTimesArrested += value;
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnRevived()
	{
		super.OnRevived();
		m_iShocksDelivered = 0;
		m_iTimesArrested = 0;
	}
}