modded class ACE_Medical_CardiovascularComponent
{
	// Needs replication
	[RplProp()]
	protected ACE_Medical_ECardiacRhythm m_eCardiacRhythm = ACE_Medical_ECardiacRhythm.Sinus;
	
	protected static const ref array<ACE_Medical_ECardiacRhythm> pulselessRhythms = { ACE_Medical_ECardiacRhythm.VF, ACE_Medical_ECardiacRhythm.PEA };
	
	// Needs replication
	[RplProp()]
	protected int m_iShocksDelivered = 0;
	protected float m_fShockChanceIncrease = 0.25;
	[RplProp()]
	protected float m_fShockCooldown = 0;
	protected float m_fShockCooldownTime = 120;
	
	override void Revive()
	{
		super.Revive();
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_Medical_ECardiacRhythm GetCardiacRhythm()
	{
		return m_eCardiacRhythm;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetCardiacRhythm(ACE_Medical_ECardiacRhythm rhythm)
	{
		m_eCardiacRhythm = rhythm;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	float GetShockCooldownTime()
	{
		return m_fShockCooldownTime;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetShockCooldown()
	{
		return m_fShockCooldown;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetShockCooldown(float value, bool replicate = false)
	{
		m_fShockCooldown = value;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	float GetShockChanceIncrease()
	{
		return m_fShockChanceIncrease;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetShocksDelivered()
	{
		return m_iShocksDelivered;
	}
	
	//------------------------------------------------------------------------------------------------
	void AddShocksDelivered(int value)
	{
		m_iShocksDelivered = m_iShocksDelivered + 1;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	void ResetShocksDelivered()
	{
		m_iShocksDelivered = 0;
		Replication.BumpMe();
	}
}