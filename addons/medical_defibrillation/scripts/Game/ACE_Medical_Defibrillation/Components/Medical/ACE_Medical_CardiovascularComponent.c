modded class ACE_Medical_CardiovascularComponent
{
	protected ACE_MedicalDefibrillation_DefibrillationSystemSettings m_DefibrillationSettings;
	
	// Needs replication
	[RplProp()]
	protected ACE_MedicalDefibrillation_ECardiacRhythm m_eCardiacRhythm = ACE_MedicalDefibrillation_ECardiacRhythm.Sinus;
	
	protected static const ref array<ACE_MedicalDefibrillation_ECardiacRhythm> pulselessRhythms = { ACE_MedicalDefibrillation_ECardiacRhythm.VF, ACE_MedicalDefibrillation_ECardiacRhythm.PEA };
	
	// Needs replication
	[RplProp()]
	protected int m_iShocksDelivered = 0;
	protected float m_fShockChanceIncrease = 0.25;
	[RplProp()]
	protected float m_fShockCooldown = 0;
	protected int m_iShockCooldownTime = 120;
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		ACE_Medical_Settings settings = ACE_SettingsHelperT<ACE_Medical_Settings>.GetModSettings();
		if (settings)
		{
			m_DefibrillationSettings = settings.m_DefibrillationSystem;
			m_iShockCooldownTime = m_DefibrillationSettings.m_iShockCooldown;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void Revive()
	{
		super.Revive();
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_MedicalDefibrillation_ECardiacRhythm GetCardiacRhythm()
	{
		return m_eCardiacRhythm;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetCardiacRhythm(ACE_MedicalDefibrillation_ECardiacRhythm rhythm)
	{
		m_eCardiacRhythm = rhythm;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	float GetShockCooldownTime()
	{
		return (float)m_iShockCooldownTime;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetShockCooldown()
	{
		return m_fShockCooldown;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetShockCooldown(float value)
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