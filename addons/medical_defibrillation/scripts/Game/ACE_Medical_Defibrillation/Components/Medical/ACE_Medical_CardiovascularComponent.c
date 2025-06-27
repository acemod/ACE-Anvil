modded class ACE_Medical_CardiovascularComponent
{
	protected ACE_MedicalDefibrillation_DefibrillationSystemSettings m_DefibrillationSettings;
	protected float m_fShockChanceIncrease = 0.25;
	protected int m_iShockCooldownTime = 120;
	
	[RplProp()]
	protected ACE_MedicalDefibrillation_ECardiacRhythm m_eCardiacRhythm = ACE_MedicalDefibrillation_ECardiacRhythm.Sinus;
	
	[RplProp()]
	protected float m_fShockCooldown = 0;
	
	[RplProp()]
	protected int m_iShocksDelivered = 0;
	
	protected static const ref array<ACE_MedicalDefibrillation_ECardiacRhythm> s_aPulselessRhythms = { ACE_MedicalDefibrillation_ECardiacRhythm.VF, ACE_MedicalDefibrillation_ECardiacRhythm.PEA };
	
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
	float GetShockChanceIncrease()
	{
		return m_fShockChanceIncrease;
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
	
	//------------------------------------------------------------------------------------------------
	override void Revive()
	{
		super.Revive();
	}
	
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
}