//------------------------------------------------------------------------------------------------
class ACE_Overheating_MuzzleContext : ACE_FrameJobScheduler_IObjectContext<ACE_Overheating_MuzzleJamComponent>
{
	bool m_bIsChamberingPossible;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Overheating_MuzzleContext(ACE_Overheating_MuzzleJamComponent object)
	{
		m_bIsChamberingPossible = object.GetMuzzle().IsChamberingPossible();
	}
}
